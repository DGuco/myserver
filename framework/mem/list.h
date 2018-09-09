//
// Created by DGuco on 18-8-28.
// 使用共享内存的定长list,list的是长度一定的,list中的对象内存地址在事先申请好的共享内存中
//

#ifndef SERVER_LIST_H
#define SERVER_LIST_H

#include <cstddef>
#include "share_container_interface.h"
#include "array_list.h"
#include <list>

namespace sharemem
{
	template<class T, size_t count>
	class list final: public container
	{
	private:
		sharemem::fixed_size_memory_pool<T, count> hash_table_;  //容器对象内存池
		std::list<T> list_;
	public:
		void initialize() override
		{
			hash_table_.initailize_pool( );
		}

		void resume() override
		{
			hash_table_.resume_pool( );
		}

	};
}
#endif //SERVER_LIST_H
