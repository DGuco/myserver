//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018Äê DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "base.h"
#include "singleton.h"

struct Student
{
	int id;
	char name[20];
	int age;
};

#define MAX_STUDENT_NUM 1000
class CDBCtrl : public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	~CDBCtrl();

private:
	Student m_student[MAX_STUDENT_NUM];
};

#endif


