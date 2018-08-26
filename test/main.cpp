//
// Created by dguco on 18-6-18.
//

#include <iostream>
#include <memory>
#include "../framework/log/log.h"
using namespace std;

class Parent
{
public:
    Parent()
    {
        a = 100;
    }
    /*virtual*/ ~Parent()
    {
        printf("~Parent\n");
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
    /*virtual*/~Child()
    {
        printf("~Child\n");
    }

public:
    virtual void Say()
    {
        printf("I am Child\n");
    }
};

int main()
{
    {
        shared_ptr<Parent> sharedPtr = std::make_shared<Child>() /*= std::make_shared<Child>()*/;
        sharedPtr->Say();
    }
    printf("---------------------------------------------------\n");
    Parent *child = new Child;
    child->Say();
    delete child;
    child = NULL;
    return 0;
}