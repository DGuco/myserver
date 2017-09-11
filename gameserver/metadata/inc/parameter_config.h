#ifndef __PARAMETER_CONFIG_H__
#define __PARAMETER_CONFIG_H__

#include <string>
#include "config_data.h"

#include <map>
#include <vector>

class ParameterItem
{
    
public:
    
    ParameterItem();
    
    ~ParameterItem();
    
public:
    
    int _type;
    
    int _integer;
    
    float _floater;
    
    double _doubler;
    
    std::string _str;
    
    std::map<std::string, std::vector<int>* > _common;
    
    std::vector<int> _vi;
    
    std::map<int, int> _nomap;
    
    std::map<int, float> _nomapf;
};

class ParameterConfig : public ConfigData
{
    
public:
    
    ParameterConfig();
    
    ~ParameterConfig();
    
    int parse();
    
    void clear();
    
    ParameterItem* item(const std::string& key);
    
public:
    
    std::map<std::string, ParameterItem*> _datas;
};


#endif // __PARAMETER_CONFIG_H__

