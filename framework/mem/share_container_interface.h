//
// Created by dguco on 18-8-27.
// 使用共享内存容器,热起动
//

#ifndef SERVER_SHARE_CONTAINER_H
#define SERVER_SHARE_CONTAINER_H
namespace sharemem
{
	class container
	{
	public:
		virtual void initialize() = 0;
		virtual void resume() = 0;
	};
}

#endif //SERVER_SHARE_CONTAINER_H
