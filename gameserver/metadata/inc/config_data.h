#ifndef __CONFIG_DATA_H__
#define __CONFIG_DATA_H__

#include "../../framework/framework.h"

#define CONFIG_DATA_CLEAR(obj) for(auto it=obj.begin();it!=obj.end();++it){delete it->second;}obj.clear();

class ConfigData
{
    
public:
    
    ConfigData();
    
    virtual ~ConfigData();
    
    int loadFromFile(const std::string& filename);
    
    virtual int parse();
    
    virtual void clear();
    
    virtual bool check();               //检测配置正确性
    
    int reload();
    
public:
    
    json::Value m_obj;
    
    static std::string data_dir;
    std::string _filename;
};

#endif // __CONFIG_DATA_H__

