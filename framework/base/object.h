//
//  object.h
//  游戏对象父类
//  Created by DGuco on 16/12/19.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#ifndef _OBJECT_H_
#define _OBJECT_H_

//// 对象创建模式
//enum
//{
//	Init	= 0,	// 初始化模式
//	Resume	= 1,	// 恢复模式
//};


#include "base.h"

#define OBJ_ID_BITS (56)  // 对象ID的范围最大为2^^56,
#define INVALID_OBJ_ID	0 // 无效对象ID
typedef __uint64_t  OBJ_ID;

class CObj
{
public:
	CObj()
		/*: id_( 0 )*/
	{
	}

	virtual ~CObj()
	{
		// 在析构的时候，设置id_，可以保证内存中id_数据的合法性
		// 这是因为有时候需要通过id来访问数据，而在共享内存中的数据，并不会真正析构
		id_ = INVALID_OBJ_ID;
	}

	OBJ_ID get_id() const
	{
		return id_;
	}

	void set_id(OBJ_ID id)
	{
		id_ = id;
	}

	static int ID2TYPE(OBJ_ID id)
	{
		return (id >> OBJ_ID_BITS) & 0X00000000000000FF;
	}

	// 判断object是否合法
	// 由于开启异常处理不太方便，在这里加上这个函数。如果构造函数失败，只需要设置id =INVALID_OBJ_ID即可
	// 这种方式虽然比较丑类,但是可以比较有效的不使用异常来处理构造函数中发现的错误
	int valid() const
	{
		return INVALID_OBJ_ID != id_;
	}

	// 设置对象非法
	void invalid()
	{
		id_ = INVALID_OBJ_ID;
	}

	// 初始化对象数据( 构造函数中根据共享内存启动模式调用 )
	virtual int Initialize() = 0;

	// 恢复对象数据-继续保留内存中的数据,用于共享内存热启动 ( 构造函数中根据共享内存启动模式调用 )
	virtual int Resume() = 0; 


private:
	OBJ_ID id_; // 所有对象的唯一标识

public:
	static char msCreateMode; // 对象创建方式
};                                               
                                                           
#endif // _OBJECT_H_
