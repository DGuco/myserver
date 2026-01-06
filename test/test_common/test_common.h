#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#include <stdio.h>

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


#endif