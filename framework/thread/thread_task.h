/*****************************************************************
* FileName:thread_task.h
* Summary :
* Date	  :2024-7-10
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__
#include <functional>
#include <string>
#include <tuple>
#include <memory>
#include "log.h"

#define EMPTY_VOID_FUNC = []{}

template<size_t NUM_PARAMS,typename return_type, typename... Args>
struct TaskCaller
{
};

template<typename return_type, typename... Args>
struct TaskCaller<0, return_type,Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func,std::tuple<Args...> args)
	{
		return func();
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<1, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func,std::tuple<Args...> args)
	{
		return func(std::get<0>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<2, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
					std::get<1>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<3, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
					std::get<1>(args),
					std::get<2>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<4, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args), 
					std::get<1>(args),
					std::get<2>(args), 
					std::get<3>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<5, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args), 
					std::get<1>(args), 
					std::get<2>(args),
					std::get<3>(args), 
					std::get<4>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<6, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			        std::get<1>(args),
					std::get<2>(args),
					std::get<3>(args),
					std::get<4>(args),
					std::get<5>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<7, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
					std::get<1>(args),
					std::get<2>(args),
					std::get<3>(args),
					std::get<4>(args),
					std::get<5>(args),
					std::get<6>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<8, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<9, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<10, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<11, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args),
			std::get<10>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<12, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<13, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<14, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args),
			std::get<13>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<15, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...> args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args),
			std::get<13>(args),
			std::get<14>(args));
	}
};

class CThreadTask : public enable_shared_from_this<CThreadTask>
{
public:
	CThreadTask(std::string signature) : m_TaskSignature(signature) {};
	virtual ~CThreadTask() {};
	void SetStartTime(time_t time)				{ m_nExecuteStart = time; }
	void SetEndTime(time_t time)				{ m_nExecuteEnd = time; }
	time_t GetStartTime()						{ return m_nExecuteStart; }
	time_t GetEndTime()							{ return m_nExecuteEnd; }
	const std::string& GetSignature()			{ return m_TaskSignature; }
	std::shared_ptr<CThreadTask> getShared()	{ return shared_from_this(); }
	virtual void Execute() = 0;
	virtual void OnFinish() = 0;
	virtual void OnFailed() = 0;
private:
	std::string m_TaskSignature;	//����ǩ��
	time_t	    m_nExecuteStart;	//����ʼִ��ʱ��
	time_t	    m_nExecuteEnd;		//����ִ�����ʱ��
};

class CVoidTask : public CThreadTask
{
public:
	CVoidTask(std::string signature,
			  std::function<void()> func,
			  std::function<void()> callfunc) : CThreadTask(signature)
	{
		m_Func = func;
		m_CallBack = callfunc;
	}
	virtual ~CVoidTask()
	{}
	virtual void Execute()
	{
		m_Func();
	}
	virtual void OnFinish()
	{
		m_CallBack();
	}
	virtual void OnFailed()
	{}
private:
	std::function<void()> m_Func;
	std::function<void()> m_CallBack;
};

template<class Func, class...Args>
class CParamTask : public CThreadTask
{
	enum
	{
		//��������
		arity = sizeof...(Args)
	};
	using ArgsTubleType = typename std::tuple<Args...>;
	using return_type = typename std::result_of<Func(Args...)>::type;
	using function_type = typename std::function<return_type(Args...)>;
public:
	CParamTask(std::string signature,const Func& func,ArgsTubleType& args)
		: CThreadTask(signature)
	{
		m_Func = func;
		m_ArgTuple = std::move(args);
	}
	virtual ~CParamTask()
	{}
	virtual void Execute()
	{
		m_Res = TaskCaller<arity, return_type, Args...>::invoke(m_Func, m_ArgTuple);
	}
	virtual void OnFinish()
	{}
	virtual void OnFailed()
	{}
private:
	function_type			m_Func;
	ArgsTubleType			m_ArgTuple;
	return_type				m_Res;
	std::function<void()>	m_CallBack;
};
#endif //__THREAD_TASK_H__
