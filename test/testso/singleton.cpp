//
// Created by dguco on 18-8-22.
//
#include <memory>
#include "singleton.h"

CTest::CTest()
{
	printf("CTest::CTest\n");
}

CTest::~CTest()
{
	printf("CTest::~CTest\n");
}

void CTest::Say()
{
	printf("Hello Java\n");
}
