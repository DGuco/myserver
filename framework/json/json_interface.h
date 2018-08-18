//
//  json.h
//  读取json文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _JSON_H_
#define _JSON_H_

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <vector>
#include <string>
#include <set>

using namespace std;
using rapidjson::Value;

class ABJson
{
public:
	ABJson();
	virtual ~ABJson();
	int LoadFromFile(std::string filename);
	virtual int Parse();
	virtual void Clear();
	int Reload();
protected:
	rapidjson::Document m_Obj;
	std::string m_sFilename;
};

#endif //_JSON_H_