#include "config_data.h"
#include "../global.h"
#include <fstream>
#include <iostream>
#include <sstream>



ConfigData::ConfigData()
{
    _filename.clear();
}

ConfigData::~ConfigData()
{
    m_obj.Clear();
    _filename.clear();
}

std::string ConfigData::data_dir = "";

int ConfigData::loadFromFile(const std::string &filename)
{
    _filename = filename;
    try {
        std::fstream fp;
        std::string filepath = data_dir + filename;
        fp.open(filepath.c_str());
        if (!fp.is_open()) {
            ERROR_LOG("config file not exist: {}", filepath);
            return -1;
        }
        std::string data;
        std::string linedata;
        std::string word;
        
        while (!fp.eof()) {
            getline(fp, linedata, '\n');
            data += linedata;
        }
        fp.close();
        
        m_obj.Clear();
        m_obj = json::Deserialize(data);
        std::string jstr = json::Serialize(m_obj);
        clear();
        int ret = parse();

//        if (filename != "server.json"){
//            INFO_LOG("load config data: {}", filename);
//        }

        return ret;
    } catch (...) {
        return -1;
    }
}

int ConfigData::parse()
{
    return 0;
}

void ConfigData::clear()
{
    
}

bool ConfigData::check()
{
    return true;
}

int ConfigData::reload()
{
    clear();
    return loadFromFile(_filename);
}
