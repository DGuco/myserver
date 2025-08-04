/*****************************************************************
* FileName:lock_free_queue.h
* Summary :无锁环形队列 固定长度
* Date	  :2024-6-5
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __LOCK_FREE_LIMIT_QUEUE_H__
#define __LOCK_FREE_LIMIT_QUEUE_H__

namespace my_std
{
	struct LockFreeResult 
	{
		bool success = false;
		bool notify = false;
	};

	// 无锁环形队列
	template <typename T, typename SizeType = size_t>
	class LockFreeLimitQueue
	{
	public:
		typedef SizeType uint_t;
		typedef std::atomic<uint_t> atomic_t;

		// 多申请一个typename T的空间, 便于判断full和empty.
		explicit LockFreeLimitQueue(uint_t capacity)
			: capacity_(capacity)
			, readable_(0 )
			, write_(0 )
			, read_(0 )
			, writable_(capacity)
		{
			buffer_ = (T*)malloc(sizeof(T) * capacity_);
		}

		~LockFreeLimitQueue() 
		{
			// destory elements.
			uint_t read = consume(read_);
			uint_t readable = consume(readable_);
			for (; read < readable; ++read) 
			{
				buffer_[read].~T();
			}

			free(buffer_);
		}

		template <typename U>
		LockFreeResult Push(U&& t) 
		{
			LockFreeResult result;

			/* 初始状态 
				--      --      --      --      --      --      --      --      --      --
		      r_|ra_|w_												                    wa_ 
			*/
			// 1.write_步进1.先占一个可写的位置
			uint_t write, writable;
			do {
				write = relaxed(write_);
				writable = consume(writable_);
				//满了，无法继续push
				if (write == writable)
					return result;

			} while (!write_.compare_exchange_weak(write, mod(write + 1),
				std::memory_order_acq_rel, std::memory_order_relaxed));

#pragma push_macro("new")
#undef new
			new (buffer_ + write) T(std::forward<U>(t));
#pragma pop_macro("new")
			/* 此时状态，注意此时write并没有重新读取 write = write_ - 1
				--      --      --      --      --      --      --      --      --      --
		       r_|ra_   w_											                    wa_ 
			*/
			// 2.数据写入

			// 3.更新readable_，标记此位置可以pop
			uint_t readable;
			do {
				readable = relaxed(readable_);
			} while (!readable_.compare_exchange_weak(readable, mod(readable + 1),
				std::memory_order_acq_rel, std::memory_order_relaxed));

			/* 最终状态，注意此时readable并没有重新读取 readable = ra_ - 1 
				--      --      --      --      --      --      --      --      --      --
		        r_     ra_|w_											                wa_ 
			*/
			// 4.检查写入时是否empty
			result.notify = (write == mod(writable + 1));
			result.success = true;
			return result;
		}

		LockFreeResult Pop(T& t) {
			LockFreeResult result;

			/* 初始状态 
				--      --      --      --      --      --      --      --      --      --
		        r_             ra_|w_								                    wa_ 
			*/
			// 1.read_步进1.
			uint_t read, readable;
			do {
				read = relaxed(read_);
				readable = consume(readable_);
				//空了，没有元素可以pop了
				if (read == readable)
					return result;

			} while (!read_.compare_exchange_weak(read, mod(read + 1),
				std::memory_order_acq_rel, std::memory_order_relaxed));

			// 2.读数据
			t = std::move(buffer_[read]);
			buffer_[read].~T();

			/* 此时状态 ，注意此时read并没有重新读取 read = read_ - 1 
				--      --      --      --      --      --      --      --      --      --
		               r_     ra_|w_								                    wa_ 
			*/
			// 3.更新writable_,空出已经出栈的位置
			uint_t writable;
			do {
				writable = relaxed(writable_);
			} while (!writable_.compare_exchange_weak(writable, mod(writable + 1),
				std::memory_order_acq_rel, std::memory_order_relaxed));

			/* 此时状态 
				--      --      --      --      --      --      --      --      --      --
		        wa_     r_     ra_|w_								                    
			*/
			// 4.检查读取时是否full
			result.notify = (read == mod(readable_ + 1));
			result.success = true;
			return result;
		}

	private:
		inline uint_t relaxed(atomic_t& val) {
			return val.load(std::memory_order_relaxed);
		}

		inline uint_t acquire(atomic_t& val) {
			return val.load(std::memory_order_acquire);
		}

		inline uint_t consume(atomic_t& val) {
			return val.load(std::memory_order_consume);
		}

		inline uint_t mod(uint_t val) {
			return val % capacity_;
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
	};
}
#endif //__LOCK_FREE_LIMIT_QUEUE_H__
