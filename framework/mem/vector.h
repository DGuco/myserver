//
// Created by dguco on 18-8-28.
// 使用共享内存的定长vector,vector的是长度一定的,vector中的对象内存地址在事先申请好的共享内存中
//

#ifndef SERVER_VECTOR_H
#define SERVER_VECTOR_H

#include "share_container_interface.h"
#include "array_list.h"
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

/**
 *  sharemem,value_type 非指针非引用
 */
namespace sharemem
{
	template<typename T, std::size_t count>
	class vector final: public container
	{
	private:
		std::vector<T> vector_;
	public:
		void initialize() override
		{

		}
		void resume() override
		{

		}

		vector()
		{
			if (std::is_pointer<T>::value || std::is_reference<T>::value) {
				throw std::invalid_argument("the value type of share vector is illegal");
			}
		}
	};
}
#endif //SERVER_VECTOR_H
