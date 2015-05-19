#ifndef __MODBUS_H
#define	__MODBUS_H
#include "public.h"


u16bit calccrc(u8bit crcbuf,u16bit crc);
u16bit chkcrc(uchar *buf,uchar len);
u16bit CRC16(u8bit *updata,u16bit len);



#endif

