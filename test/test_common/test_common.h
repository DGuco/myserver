#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#include <stdio.h>
#include "safe_pointer.h"
#include "t_array.h"

#define SafePointer     CSafePtr
#define VOID			void			//标准空
typedef unsigned char	UCHAR;			//标准无符号CHAR
typedef char			CHAR;			//标准CHAR
typedef unsigned int	UINT;			//标准无符号INT
typedef int				INT;			//标准INT
typedef unsigned short	USHORT;			//标准无符号short
typedef short			SHORT;			//标准short
typedef unsigned long	ULONG;			//标准无符号LONG(不推荐使用)
typedef long			LONG;			//标准LONG(不推荐使用)
typedef float			FLOAT;			//标准float

typedef UCHAR			uchar;
typedef USHORT			ushort;
typedef UINT			uint;
typedef ULONG			ulong;
typedef ULONG			IP_t;
typedef USHORT			PacketID_t;
typedef INT				BOOL;
typedef UCHAR			BYTE;

#define __ENTER_FUNCTION try {
#define __LEAVE_FUNCTION } catch (...) { }

#define Assert(x) if(!(x)) { printf("Assert failed: %s, line %d\n", __FILE__, __LINE__); }
#define Disk64Log(a,b)  {;} 


class SocketInputStream
{
public:
    template<typename T>
    bool Read(T &val,int len)
    {
        return true;
    }
};

class SocketOutputStream 
{
public:
    template<typename T>
    bool Write(const T &val,int len)
    {
        return true;
    }
};

//数据段
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

struct GUID64_t
{
    GUID64_t() {  m_High = 0; m_Low = 0;}
    GUID64_t(int high, int low) {     m_High = high;  m_Low = low; }
    bool operator==(const GUID64_t &other) const { return m_High == other.m_High && m_Low == other.m_Low; }
    bool operator!=(const GUID64_t &other) const {  return !(*this == other); }
    bool operator<(const GUID64_t &other) const  {   return m_High < other.m_High || (m_High == other.m_High && m_Low < other.m_Low);}
    bool operator>(const GUID64_t &other) const  {   return other < *this; }
    int m_High;
    int m_Low;

    bool Read(SocketInputStream& inputStream)
    {
        inputStream.Read(m_High, sizeof(m_High));
        inputStream.Read(m_Low, sizeof(m_Low));
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        outputStream.Write(m_High, sizeof(m_High));
        outputStream.Write(m_Low, sizeof(m_Low));
        return true;
    }
};

class Obj_Human
{
public:
    Obj_Human()
    {}
    ~Obj_Human()
    {}

    VOID SendNoticeMsg(const CHAR *szMsg)
    {}
public:
    GUID64_t m_GUID;
};

#endif