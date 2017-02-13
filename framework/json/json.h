#ifndef _JSON_H_
#define _JSON_H_

#include "../include/rapidjson/rapidjson.h"
#include "../include/rapidjson/document.h"
namespace MyJson {
    class Json {
    public:

        Json();

        virtual ~Json();

        int loadFromFile(const std::string &filename);

        virtual int parse();

        virtual void clear();

        int reload();

    public:

        rapidjson::Value m_obj;

        std::string _filename;
    };

#endif // __CONFIG_DATA_H__

#endif //_JSON_H_
}