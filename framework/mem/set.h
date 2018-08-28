//
// Created by dguco on 18-8-28.
// 使用共享内存的定长set,set的是长度一定的,set中的对象内存地址在事先申请好的共享内存中
//

#ifndef SERVER_SET_H
#define SERVER_SET_H
/**
 * sharemem,value_type 非指针非引用
 */
#include <cstddef>
#include <set>
#include "share_container_interface.h"
#include "array_list.h"
#include "set.h"
namespace sharemem
{
	template<class _Val, std::size_t _Cap>
	class set final: public container
	{
	public:
		typedef _Val value_type_;

		/**
		 * 注意迭代起类型是value_type* 类型
		 */
		typedef fixed_size_memory_pool::pointer pointer;
		typedef typename std::set<pointer>::iterator iterator;
		typedef typename std::set<pointer>::const_iterator const_iterator;
	private:
		sharemem::fixed_size_memory_pool<value_type_, _Cap> hash_table_;  //容器对象内存池
		std::set<pointer> set_;  //key value hash
	public:
		set(const set &other)
		{
			if (this == &(other)) {
				return;
			}
			this->hash_table_ = other.hash_table_;
			this->set_ = other.set_;
		}
		//no move
		set(set &&other) = delete;
		set(const set &&other) = delete;

		set &operator=(set &other)
		{
			if (this == &(other)) {
				return *this;
			}
			this->hash_table_ = other.hash_table_;
			this->set_ = other.set_;
			return *this;
		};

		set &operator=(set &&other)
		{
			if (this == &(other)) {
				return *this;
			}
			this->hash_table_ = other.hash_table_;
			this->set_ = other.set_;
			return *this;
		};

		set()
		{
			if (std::is_pointer<_Val>::value || std::is_reference<_Val>::value) {
				throw std::invalid_argument("the value type of share set is illegal");
			}
		}
		~set()
		{
			set_.clear( );
		}

		void initialize() override
		{
			set_.clear( );
			hash_table_.initailize_pool( );
		}

		void resume() override
		{
			set_.clear( );
			hash_table_.resume_pool( );
			//重新生成hash
			for (int i = 0; i < hash_table_.size( ); i++) {
				value_type_ *it = hash_table_[i];
				set_.insert(std::make_pair(it->key, &it->value));
			}
		}

		/**
		 * 插入元素
		 * @param key
		 * @param value
		 * @return 插入是否成功
		 */
		bool insert(value_type_ &value)
		{
			pointer it = hash_table_.allocate(value);
			MY_ASSERT_STR(it != nullptr, return false, "fixed_size_memory_pool allocate failed");
			if (nullptr != it) {
				return set_.insert(it).second;
			}
			else {
				return false;
			}
		}

		iterator begin()
		{
			return set_.begin( );
		}

		const_iterator begin() const
		{
			return set_.begin( );
		}

		iterator end()
		{
			return set_.end( );
		}

		const_iterator end() const
		{
			return set_.end( );
		}

//		iterator find(value_type_ &key)
//		{
//			hash_table_.
//			return set_.find(key);
//		}
//
//		const_iterator find(const key_type_ &key)
//		{
//			return set_.find(key);
//		}
//
//		size_t erase(const key_type_ &key)
//		{
//			iterator it = set_.find(key);
//			if (it != set_.end( )) {
//				hash_table_.deallocate(it->second);
//				return set_.erase(key);
//			}
//			return 0;
//		}
//
//		iterator erase(const_iterator __position)
//		{
//			hash_table_.deallocate(__position->second);
//			return set_.erase(__position);
//		}
//
//		iterator erase(iterator __position)
//		{
//			hash_table_.deallocate(__position->second);
//			return set_.erase(__position);
//		}
//
//		/**
//		 * Erases a [first,last) range of elements from a %set.
//		 */
//		iterator
//		erase(const_iterator __first, const_iterator __last)
//		{
//			hash_table_.deallocate(__first->second, __last->second);
//			return set_.erase(__first, __last);
//		}

		size_t size()
		{
			return set_.size( );
		}

		void clear()
		{
			set_.clear( );
			hash_table_.initailize_pool( );
		}

	};
}
#endif //SERVER_SET_H
