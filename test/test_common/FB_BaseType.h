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
typedef INT				ScriptID_t;			//脚本

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

    bool Write(SocketOutputStream& outputStream) const
    {
        outputStream.Write(m_High, sizeof(m_High));
        outputStream.Write(m_Low, sizeof(m_Low));
        return true;
    }
};

class LuaInterface
{
public:
	LuaInterface();
	~LuaInterface();
	INT		ExeScript( ScriptID_t scriptid, CHAR* funcname ) {return 0;};
	INT		ExeScript_D( ScriptID_t scriptid, CHAR* funcname, INT Param0 ) {return 0;};
	INT		ExeScript_DD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1 ) {return 0;};
	INT		ExeScript_DDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2 ) {return 0;};
	INT		ExeScript_DDDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2, INT Param3 ) {return 0;};
	INT		ExeScript_DDDDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2, INT Param3, INT Param4 ) {return 0;};
	INT		ExeScript_DDDDDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2, INT Param3, INT Param4, INT Param5 ) {return 0;};
	INT		ExeScript_DDDDDDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2, INT Param3, INT Param4, INT Param5, INT Param6 ) {return 0;};
	INT		ExeScript_DDDDDDDD( ScriptID_t scriptid, CHAR* funcname, INT Param0, INT Param1, INT Param2, INT Param3, INT Param4, INT Param5, INT Param6, INT Param7 ) {return 0;};
};

class Scene
{
public:
    LuaInterface* GetLuaInterface() { return NULL; }
    INT           SceneID() { return 0; }
    BOOL VerifyExecuteThread() { return true; }
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

    GUID64_t GetGUID() { return m_GUID; }
    Scene* getScene() { return NULL; }

public:
    GUID64_t m_GUID;
};

class GamePlayer
{
public :
	GamePlayer( BOOL bIsServer=FALSE ) ;
	~GamePlayer( ) ;
    Obj_Human* GetHuman() { return NULL; }
};

class Packet;
class Player : public GamePlayer
{
public :
	Player( BOOL bIsServer=FALSE ) ;
	~Player( ) ;
	virtual BOOL SendPacket( Packet* pPacket ) ;
};

class PacketFactory 
{
public :
	virtual ~PacketFactory ()  {}
	virtual Packet*		CreatePacket ()  = 0;
	virtual PacketID_t	GetPacketID ()const  = 0;
	virtual UINT		GetPacketMaxSize ()const  = 0;
};

class Packet
{
public :
	Packet( ) {};
	virtual ~Packet( ) {} ;
	virtual VOID	CleanUp( ){} ;
	virtual BOOL	Read( SocketInputStream& iStream ) = 0 ;
	virtual BOOL	Write( SocketOutputStream& oStream ) const = 0;
	virtual UINT		Execute( Player* pPlayer ) = 0 ;
	virtual	PacketID_t	GetPacketID( ) const = 0 ;
	virtual	UINT		GetPacketSize( ) const = 0 ;
};

enum PACKET_EXE
{
	PACKET_EXE_ERROR = 0 ,
	PACKET_EXE_BREAK ,
	PACKET_EXE_CONTINUE ,
	PACKET_EXE_NOTREMOVE ,
	PACKET_EXE_NOTREMOVE_ERROR ,
};


#endif