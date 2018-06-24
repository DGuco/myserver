//
// Created by dguco on 18-6-18.
//

#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
using namespace std;

class Parent
{
public:
    ~Parent()
    {
        printf("~Parent\n");
    }
public:
    virtual void Say()
    {
        printf("I am Parent\n");
    }
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
    printf("Use count = %d\n", sharedPtr.use_count());
}

int main()
{
    Child *child = new Child;
    Parent *parent = child;
    parent->Say();
    delete parent;
//    shared_ptr<Parent> sharedPtr = std::make_shared<Child>();
//    sharedPtr->Say();
//    test(sharedPtr);
//    printf("Use count = %d\n", sharedPtr.use_count());
    return 0;
}