//
// Created by dguco on 18-9-3.
// 配置文件
//

#ifndef SERVER_CONFIG_HANDLE_H
#define SERVER_CONFIG_HANDLE_H

#include <unordered_map>
#include <string>
#include <memory.h>
#include <memory>
#include "json_interface.h"


const static string configPath = "../config/";

using namespace std;
class CConfigHandle
{
public:
	int PrepareToRun();
	void Resume();
	void Reload(const string &fileName);
private:
	void LoadFile(const string &fileName);
private:
	std::unordered_map<string, std::shared_ptr<AJson>> m_mConfigMap;
};
#endif //SERVER_CONFIG_HANDLE_H
