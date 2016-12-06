#ifndef __CONFIG_DATA_H__
#define __CONFIG_DATA_H__

#include <string>
#include "json.h"
using namespace std;
using namespace json;
#define CONFIG_DATA_CLEAR(obj) for(auto it=obj.begin();it!=obj.end();++it){delete it->second;}obj.clear();

class ConfigData
{
    
public:
    
    ConfigData();
    
    virtual ~ConfigData();
    
    int loadFromFile(const std::string& filename);
    
    virtual int parse();
    
    virtual void clear();
    
    int reload();
    
public:
    
    json::Value m_obj;
    
    std::string _filename;
};

#endif // __CONFIG_DATA_H__

