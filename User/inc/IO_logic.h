#ifndef __IO_LOGIC_H_
#define __IO_LOGIC_H_
#include "public.h"
#include "DrvGPIO.h"

#define	RDY		GPA_11
#define	PBF		GPA_10
#define	S_RTS	GPA_9
#define	REQ_IT	GPA_8

void GPIOInit(void);
u32bit GetUart1Baud(void);
#endif



