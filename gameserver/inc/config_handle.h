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
#include "net_work.h"


const static string configPath = "../config/";

using namespace std;
class CConfigHandle
{
public:
    CConfigHandle();
    int PrepareToRun();
	void Resume();
	void Reload(const string &fileName);
    //配置文件改变回调
    static void lcb_OnConfigChanged(inotify_event *notifyEvent);
private:
	void LoadFile(const string &fileName);
private:
    std::shared_ptr<CNetWork> m_pNetWork;
	std::unordered_map<string, std::shared_ptr<AJson>> m_mConfigMap;
};
#endif //SERVER_CONFIG_HANDLE_H
