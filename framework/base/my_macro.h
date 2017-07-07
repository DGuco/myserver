//
// Created by DGcuo on 17-7-5.
//

#ifndef SERVER_MY_NACRO_H
#define SERVER_MY_NACRO_H

#define MAP_SAGE_FIND(map, key) 	\
	do 								\
	{ 								\
        auto it = map.find(key);    \
        if (it != map.end())        \
        {                           \
            return it->second;      \
        }                           \
        return NULL;                \
	} while(0);

#define SAFE_DELETE(pointer)        \
    do                              \
    {                               \
        if (pointer != NULL)        \
        {                           \
            delete pointer;         \
        }                           \
    }while(0);

#endif //SERVER_MY_NACRO_H
