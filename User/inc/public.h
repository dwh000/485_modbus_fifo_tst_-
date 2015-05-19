#ifndef __public_h
	#define	__public_h
#include <stdbool.h>
#include "DrvSYS.h"	

    /* exact-width signed integer types */
typedef   signed          char schar;		//8bit
typedef   signed short     int sint;		//16bit
typedef   signed           int slong;		//32bit
typedef   signed       __int64 slonglong;	//64bit

    /* exact-width unsigned integer types */
typedef unsigned          char uchar;		//8bit
typedef unsigned short     int ushort;		//16bit
typedef unsigned           int ulong;		//32bit
typedef unsigned       __int64 ulonglong;	//64bit

    /* exact-width signed integer types */
typedef   signed          char s8bit;		//8bit
typedef   signed short     int s16bit;		//16bit
typedef   signed           int s32bit;		//32bit
typedef   signed       __int64 s64bit;		//64bit

    /* exact-width unsigned integer types */
typedef unsigned          char u8bit;		//8bit
typedef unsigned short     int u16bit;		//16bit
typedef unsigned           int u32bit;		//32bit
typedef unsigned       __int64 u64bit;		//64bit

#define DelayUs(x)	DrvSYS_Delay(x)
#endif

