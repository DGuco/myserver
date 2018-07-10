//
// Created by dguco on 18-6-18.
//

#include <iostream>
#include <memory>
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

Parent &test1()
{
//	Parent *paren = new Parent;
	Parent parent;
	return parent;
}

int main()
{
	Parent &parent = test1();
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