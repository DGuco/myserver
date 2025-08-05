/*****************************************************************
* FileName:lock_free_queue.h
* Summary :无锁环形队列 固定长度
* Date	  :2024-6-5
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __LOCK_FREE_LIMIT_QUEUE_H__
#define __LOCK_FREE_LIMIT_QUEUE_H__

#include "my_assert.h"
#include "my_lock.h"

namespace my_std
{
	struct LockFreeResult 
	{
		bool success = false;
		bool notify = false;
	};

	// 无锁环形队列
	template <typename T,typename size_t Cap = 5>
	class LockFreeLimitQueue
	{
public:
		typedef uint64_t index_type;
		typedef std::atomic<uint64_t> atomic_t;

		struct IndexWithVersion 
		{
			uint32_t version;// 高32位存储版本号
			uint32_t index;  // 低32位存储索引

			IndexWithVersion()
			{
				index = 0;
				version = 0;
			}

			IndexWithVersion(uint64_t iv)
			{
				version = iv >> 32;
				index = iv & 0xFFFFFFFF;
			}

			IndexWithVersion(uint32_t index, uint32_t version)
			{
				this->index = index;
				this->version = version;
			}
			
			index_type value()
			{
				return (uint64_t)version << 32 | index;
			}

			uint32_t mod()
			{
				return index % Cap;
			}

			IndexWithVersion operator+(int value)
			{
				ASSERT_EX(value >= 0,"IndexWithVersion value must be >= 0");
				IndexWithVersion iv(index,version);
				int noldver = iv.index / Cap;
				iv.index += value;
				int newver = iv.index / Cap;
				if(noldver != newver)
					iv.version++;
				iv.index = iv.index % Cap;
				ASSERT_EX(iv.version < INT32_MAX,"IndexWithVersion version overflow");
				return IndexWithVersion(iv.index,iv.version);
			}

			bool operator==(const IndexWithVersion& other)
			{
				return index == other.index;
			}

			bool operator==(index_type other)
			{
				IndexWithVersion iv(other);
				return index == iv.index;
			}
		};

		explicit LockFreeLimitQueue()
			: capacity_(Cap)
			, readable_(0 )
			, write_(0 )
			, read_(0 )
			, writable_(Cap -1)
		{
			buffer_ = (T*)malloc(sizeof(T) * capacity_);
		}

		~LockFreeLimitQueue() 
		{
			// destory elements.
			IndexWithVersion read = acquire(read_);
			IndexWithVersion readable = acquire(readable_);
			
			uint32_t read_mod = read.mod();
			uint32_t readable_mod = readable.mod();
			for (; read_mod < readable_mod; ++read_mod) 
			{
				buffer_[read_mod].~T();
			}

			free(buffer_);
		}

		/*为了避免aba问题，所有的索引只会累加，在使用索引到数组的未知的时候，需要使用mod来获取实际的索引
		ABA问题 示例
		write_ = 0（原子指针）
		readable_ = 0（原子计数器）
		1 线程1执行Push操作：
		1.1 读取write_=0，readable_=0
		1.2 计算writable = mod(0 + 0) = 0
		1.3 此时发生线程切换
		1.4 线程2完成Push操作：

		2 成功CAS将write_从0更新到1
		2.1 更新readable_到1
		2.2 再次Push：CAS将write_从1更新到2
		2.3 更新readable_到2
		2.4 再次Push：CAS将write_从2更新到0（循环队列）
		2.5 更新readable_到3

		3 线程1恢复执行：
		3.1 检查write_当前值仍然是0（因循环队列）
		3.2 执行CAS(0 → 1)成功
		3.3 此时实际队列状态：
		write_=1（已被线程2修改过）
		readable_=3
		这会导致：
		1 线程1的写入位置0会覆盖线程2第三次Push写入的数据
		2 readable_计数器从3被错误地更新为1（应该为4）
		*/
		template <typename U>
		LockFreeResult Push(U&& t) 
		{

			//CSafeLock lock(lock_);
			LockFreeResult result;

			/* 初始状态 
				--      --      --      --      --      --      --      --      --      --
		      r_|ra_|w_												                    wa_ 
			*/
			// 1.write_步进1.先占一个可写的位置
			IndexWithVersion write, writable;
			index_type expected;
			do {
				write = acquire(write_);
				writable = acquire(writable_);
				expected = write.value();
				//满了，无法继续push
				if (write == writable)
					return result;
			} while (!write_.compare_exchange_weak(expected, (write + 1).value(),
				std::memory_order_seq_cst, std::memory_order_acquire));

#pragma push_macro("new")
#undef new
			// 在数据写入后添加内存屏障
			new (buffer_ + write.mod()) T(std::forward<U>(t));
			//std::atomic_thread_fence(std::memory_order_release);
#pragma pop_macro("new")
			/* 此时状态，注意此时write并没有重新读取 write = write_ - 1
				--      --      --      --      --      --      --      --      --      --
		       r_|ra_   w_											                    wa_ 
			*/
			// 2.数据写入

			// 3.更新readable_，标记此位置可以pop
			IndexWithVersion readable;
			index_type expected_ra;
			do {
				readable = acquire(readable_);
				expected_ra = readable.value();
			} while (!readable_.compare_exchange_weak(expected_ra, (readable + 1).value(),
				std::memory_order_seq_cst, std::memory_order_acquire));

			/* 最终状态，注意此时readable并没有重新读取 readable = ra_ - 1 
				--      --      --      --      --      --      --      --      --      --
		        r_     ra_|w_											                wa_ 
			*/
			// 4.检查写入时是否empty
			result.notify = (write == writable + 1);
			result.success = true;
			return result;
		}

		LockFreeResult Pop(T& t) {
			//CSafeLock lock(lock_);

			LockFreeResult result;

			/* 初始状态 
				--      --      --      --      --      --      --      --      --      --
		        r_     ra_|w_								                            wa_ 
			*/
			// 1.read_步进1.
			IndexWithVersion read, readable;
			index_type expected;
			do {
				read = acquire(read_);
				readable = acquire(readable_);
				expected = read.value();
				//空了，没有元素可以pop了
				if (read == readable)
					return result;

			} while (!read_.compare_exchange_weak(expected,(read + 1).value(),
				std::memory_order_seq_cst, std::memory_order_acquire));

			// 2.读数据
			t = std::move(buffer_[read.mod()]);
			buffer_[read.mod()].~T();

			/* 此时状态 ，注意此时read并没有重新读取 read = read_ - 1 
				--      --      --      --      --      --      --      --      --      --
		             r_|ra_|w_								                            wa_ 
			*/
			// 3.更新writable_,空出已经出栈的位置
			IndexWithVersion writable;
			index_type expected_wr;
			do {
				writable = acquire(writable_);
				expected_wr = writable.value();
			} while (!writable_.compare_exchange_weak(expected_wr, (writable + 1).value(),
				std::memory_order_seq_cst, std::memory_order_acquire));

			/* 此时状态 
				--      --      --      --      --      --      --      --      --      --
		        wa_   r_|ra_|w_								                    
			*/
			// 4.检查读取时是否full
			result.notify = (read == readable_ + 1);
			result.success = true;
			return result;
		}

	private:
		inline IndexWithVersion acquire(atomic_t& val) 
		{
			return IndexWithVersion(val.load(std::memory_order_acquire));
		}

	private:
		T* buffer_;
		CACHE_LINE_ALIGN size_t capacity_;

		// [write_, writable_] 可写区间, write_ == writable_ is full.
		// read后更新writable
		atomic_t write_;
		atomic_t writable_;

		// [read_, readable_) 可读区间, read_ == readable_ is empty.
		// write后更新readable
		atomic_t read_;
		atomic_t readable_;

		CMyLock lock_;
	};
}

#endif //__LOCK_FREE_LIMIT_QUEUE_H__