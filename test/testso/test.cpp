//
// Created by dguco on 18-8-18.
//

#include <stdio.h>
#include "singleton.h"

extern "C" int sum(int a, int b)
{
	printf("So2 sum\n");
	return a + b;
}

extern "C" int sub(int a, int b)
{
	printf("So2 sub\n");
	CTest *test = new CTest;
	test->Say();
	delete test;
	test = NULL;
	return a - b;
}

extern "C" void testFunc(CTest *test)
{
	printf("So2 test\n");
	test->Say();
}