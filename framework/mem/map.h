//
// Created by dguco on 18-8-27.
// 使用共享内存的定长map,map的是长度一定的,map中的对象内存地址在事先申请好的共享内存中
//

#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include <cstddef>
#include <utility>
#include <map>
#include <type_traits>
#include "array_list.h"
#include "share_container_interface.h"

/**
 * sharemem,value_type 非指针非引用
 */
namespace sharemem
{
	template<class _Kty, class _Ty, std::size_t _Cap>
	class map final: public container
	{
	public:
		typedef _Kty key_type_;
		typedef _Ty value_type_;

		/**
		 * 注意迭代起类型是value_type* 类型
		 */
		typedef fixed_size_memory_pool::pointer pointer;
		typedef typename std::map<key_type_, pointer>::iterator iterator;
		typedef typename std::map<key_type_, pointer>::const_iterator const_iterator;
	private:
		sharemem::fixed_size_memory_pool<value_type_, _Cap> hash_table_;  //容器对象内存池
		std::map<key_type_, pointer> map_;  //key value hash
	public:
		map(const map &other)
		{
			if (this == &(other)) {
				return;
			}
			this->hash_table_ = other.hash_table_;
			this->map_ = other.map_;
		}
		//no move
		map(map &&other) = delete;
		map(const map &&other) = delete;

		map &operator=(map &other)
		{
			if (this == &(other)) {
				return *this;
			}
			this->hash_table_ = other.hash_table_;
			this->map_ = other.map_;
			return *this;
		};

		map &operator=(map &&other)
		{
			if (this == &(other)) {
				return *this;
			}
			this->hash_table_ = other.hash_table_;
			this->map_ = other.map_;
			return *this;
		};

		map()
		{
			if (std::is_pointer<_Ty>::value || std::is_reference<_Ty>::value) {
				throw std::invalid_argument("the value type of share map is illegal");
			}
		}
		~map()
		{
			map_.clear( );
		}

		void initialize() override
		{
			map_.clear( );
			hash_table_.initailize_pool( );
		}

		void resume() override
		{
			map_.clear( );
			hash_table_.resume_pool( );
			//重新生成hash
			for (int i = 0; i < hash_table_.size( ); i++) {
				value_type_ *it = hash_table_[i];
				map_.insert(std::make_pair(it->key, &it->value));
			}
		}

		/**
		 * 插入元素
		 * @param key
		 * @param value
		 * @return 插入是否成功
		 */
		bool insert(key_type_ key, value_type_ &value)
		{
			pointer it = hash_table_.allocate(value);
			MY_ASSERT_STR(it != nullptr, return false, "fixed_size_memory_pool allocate failed");
			if (nullptr != it) {
				return map_.insert(std::make_pair(key, it)).second;
			}
			else {
				return false;
			}
		}

		iterator begin()
		{
			return map_.begin( );
		}

		const_iterator begin() const
		{
			return map_.begin( );
		}

		iterator end()
		{
			return map_.end( );
		}

		const_iterator end() const
		{
			return map_.end( );
		}

		iterator find(key_type_ &key)
		{
			return map_.find(key);
		}

		const_iterator find(const key_type_ &key)
		{
			return map_.find(key);
		}

		size_t erase(const key_type_ &key)
		{
			iterator it = map_.find(key);
			if (it != map_.end( )) {
				hash_table_.deallocate(it->second);
				return map_.erase(key);
			}
			return 0;
		}

		iterator erase(const_iterator __position)
		{
			hash_table_.deallocate(__position->second);
			return map_.erase(__position);
		}

		iterator erase(iterator __position)
		{
			hash_table_.deallocate(__position->second);
			return map_.erase(__position);
		}

		/**
		 * Erases a [first,last) range of elements from a %map.
		 */
		iterator
		erase(const_iterator __first, const_iterator __last)
		{
			hash_table_.deallocate(__first->second, __last->second);
			return map_.erase(__first, __last);
		}

		size_t size()
		{
			return map_.size( );
		}

		void clear()
		{
			map_.clear( );
			hash_table_.initailize_pool( );
		}

	};
}
#endif //SERVER_MAP_H
