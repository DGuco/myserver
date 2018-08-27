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
 * sharemem,value_type 非指针或者引用
 */
using namespace std;
namespace sharemem
{
	template<class _Kty, class _Ty, std::size_t _Cap>
	class map final: public container
	{
	public:
		typedef _Kty key_type;
		typedef _Ty value_type;
		typedef struct bucket_type
		{
			key_type key;
			value_type value;
		} bucket_type;
		typedef _Rb_tree_iterator<value_type *> iterator;
		typedef _Rb_tree_const_iterator<value_type> const_iterator;
	private:
		sharemem::fixed_size_memory_pool<bucket_type, _Cap> hash_table_;  //容器对象内存池
		std::map<key_type, value_type *> map_;  //key value hash
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
			if (is_pointer<_Ty>::value || is_reference<_Ty>::value) {
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
				bucket_type *it = hash_table_[i];
				map_.insert(std::make_pair(it->key, &it->value));
			}
		}

		/**
		 * 插入元素
		 * @param key
		 * @param value
		 * @return 插入是否成功
		 */
		bool insert(key_type key, value_type &value)
		{
			value_type *it = hash_table_.allocate(value);
			MY_ASSERT_STR(it != nullptr, return false, "fixed_size_memory_pool allocate failed");
			if (nullptr != it) {
				return map_.insert(std::make_pair(key, it)).second;
			}
			else {
				return false;
			}
		}

		/**
		 * 注意迭代起类型是value_type* 类型
		 * @return
		 */
		iterator begin()
		{
			return map_.begin( );
		}

		/**
	 	* 注意迭代起类型是value_type* 类型
	 	* @return
	 	*/
		const_iterator begin() const
		{
			return map_.begin( );
		}

		/**
 		* 注意迭代起类型是value_type* 类型
 		* @return
 		*/
		iterator end()
		{
			return map_.end( );
		}

		/**
	 	* 注意迭代起类型是value_type* 类型
	 	* @return
	 	*/
		const_iterator end() const
		{
			return map_.end( );
		}
	};
}
#endif //SERVER_MAP_H
