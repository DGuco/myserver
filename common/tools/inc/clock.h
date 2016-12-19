// ***************************************************************
//  File:	clock.h   version:  1.0
//	Date:	05/23/2008
// 	Desc:	封装时钟函数
//  
//  Copyright (C) 2008 - MY Entertainment Technology Co.,Ltd
//						All Rights Reserved 
// ***************************************************************
#ifndef __CLOCK_H__
#define __CLOCK_H__

class Clock
{
public:
    Clock();
    ~Clock();

    bool				active() const	{ return _active; }
    int 				elapsed() const;
    long				start();
    long				reset() { return start(); }
    int 				stop();

    static long			now();

private:

    long	_start;		/// 开始的时间
    int 	_elapsed;	/// 结束时经过的时间
    bool	_active;	/// 是否处于活动状态
};

#endif // __CLOCK_H__

