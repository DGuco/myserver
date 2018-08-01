//
// Created by dguco on 18-6-18.
//

#include <iostream>
#include <memory>
#include <atomic>
#include "../framework/log/log.h"
using namespace std;

class Parent
{
public:
	~Parent()
	{
		printf("~Parent\n");
	}
	Parent()
	{
		a = 100;
	}

	Parent(const Parent &tmp)
	{
		printf("Copy construct\n");
	}
public:
	virtual void Say()
	{
		printf("I am Parent\n");
	}
	int a;
};

class Child: public Parent
{
public:
	~Child()
	{
		printf("~Child\n");
	}

public:
	virtual void Say()
	{
		printf("I am Child\n");
	}
};

template<typename _Ptr>
class Test
{
public:
	Test(_Ptr *ptr)
		: ptr(ptr)
	{}

	virtual ~Test()
	{
		delete ptr;
	}
public:
	_Ptr *ptr;
};

void test(shared_ptr<Parent> &sharedPtr)
{
	if (sharedPtr) {
		printf("test==>Use count = %d\n", sharedPtr.use_count());
	}
}

std::atomic_int atomic_int1;

void add()
{
	for (int v = 0; v < 10000; v++) {
		int j = atomic_int1 + 2;
		atomic_int1.store(j);
	}
}

std::mutex mu;

std::atomic_int atomic_int2;

void add1()
{
	for (int v = 0; v < 10000; v++) {
		std::lock_guard<std::mutex> lk(mu);
		int j = atomic_int2 + 2;
		atomic_int2.store(j);
	}
}

Parent &test1()
{
//	Parent *paren = new Parent;
	Parent parent;
	return parent;
}

#define TO_NAME(a) #a

int main()
{
	Parent &parent = test1();
	int a = 1;
	std::cout << "Result: " << TO_NAME(a) << std::endl;
	atomic_int1.store(0);
	vector<std::thread> vector1;
	for (int i = 0; i < 10; i++) {
		vector1.push_back(std::move(std::thread(add)));
	}
	atomic_int2.store(0);
	vector<std::thread> vector2;
	for (int i = 0; i < 10; i++) {
		vector2.push_back(std::move(std::thread(add1)));
	}
	std::for_each(vector1.begin(), vector1.end(), std::mem_fn(&std::thread::join));
	std::for_each(vector2.begin(), vector2.end(), std::mem_fn(&std::thread::join));
	printf("atomic_int1 = %d\n", atomic_int1.load());
	printf("atomic_int2 = %d\n", atomic_int2.load());
//	shared_ptr<Parent> sharedPtr = std::make_shared<Child>();
//	sharedPtr->Say();
//	printf("Use count = %d\n", sharedPtr.use_count());
//	shared_ptr<Parent> ptr = NULL;
//	test(ptr);
//	test(sharedPtr);
//	[sharedPtr]
//	{
//		printf("Use count = %d\n", sharedPtr.use_count());
//		return sharedPtr.use_count();
//	}();
//	printf("Use count = %d\n", sharedPtr.use_count());
//	shared_ptr<Parent> sharedPtr1 = std::move(sharedPtr);
//	printf("Use count = %d\n", sharedPtr1.use_count());
	return 0;
}