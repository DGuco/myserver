#ifndef __DBCFILE_H__
#define __DBCFILE_H__

#include "test_common.h"

//Êý¾Ý¶Î
union FIELD
{
    FLOAT		fValue;
    INT			iValue;
    const CHAR*	pString;	// Just for runtime!
};

class DBCFile
{
public:
    DBCFile(int id) {};
    ~DBCFile() {};
    BOOL OpenFromTXT(const CHAR* szFileName) {return true;};
    INT GetRecordsNum() {return 0;}
    INT GetFieldsNum() {return 0;}
    FIELD* Search_Posistion(INT iRecord, INT iField) {return 0;}
};

#endif