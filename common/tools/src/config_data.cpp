#include "../inc/config_data.h"
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

int ConfigData::loadFromFile(const std::string &filename)
{
    _filename = filename;
    try {
        std::fstream fp;
        fp.open(filename.c_str());
        if (!fp.is_open()) {
            printf("config file not exist: %s", filename.c_str());
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

int ConfigData::reload()
{
    clear();
    return loadFromFile(_filename);
}
