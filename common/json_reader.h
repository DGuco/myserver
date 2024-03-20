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
#include <fstream>
#include <set>

using namespace std;
using rapidjson::Value;

class CJsonReader
{
public:
	CJsonReader()
	{}
	virtual ~CJsonReader()
	{}
	int LoadFromFile(std::string filename)
	{
		m_sFilename = filename;
		try 
		{
			std::fstream fp;
			fp.open(filename.c_str());
			if (!fp.is_open())
			{
				return -1;
			}
			std::string data;
			std::string linedata;

			while (!fp.eof()) 
			{
				getline(fp, linedata, '\n');
				data += linedata;
			}
			fp.close();
			m_Obj.Parse(data.c_str());
			int ret = Parse();
			return ret;
		}
		catch (...) 
		{
			return -1;
		}
	}
	int Reload()
	{
		Clear();
		return LoadFromFile(m_sFilename);
	}
public:
	virtual int Parse() = 0;
	virtual void Clear() = 0;
protected:
	rapidjson::Document m_Obj;
	std::string m_sFilename;
};

#endif //_JSON_H_