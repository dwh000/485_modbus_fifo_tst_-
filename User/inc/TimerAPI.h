#ifndef __TimeAPI_h__
#define __TimeAPI_h__
#include "public.h"
#include "DrvTIMER.h"
typedef struct timeDelay_t{
    int32_t              active;
    int32_t              curTick;
} TIMER_DELAY_T;


typedef struct TimerRelay_t{
	bool TimerRelayArray[16];
	union{
		bool T00;
		bool T01;
		bool T02;
		bool T03;
		bool T04;
		bool T05;
		bool T06;
		bool T07;
		bool T10;
		bool T11;
		bool T12;
		bool T13;
		bool T14;
		bool T15;
		bool T16;
		bool T17;
	}TimerRelay;
}TIMER_RELAY_T;

typedef enum{                   
	DelayT00		= 0,
	DelayT01		= 1,
	DelayT02		= 2,
	DelayT03		= 3,
	DelayT04		= 4,
	DelayT05		= 5,
	DelayT06		= 6,
	DelayT07		= 7,
	DelayT10		= 8,
	DelayT11		= 9,
	DelayT12		= 10,
	DelayT13		= 11,
	DelayT14		= 12,
	DelayT15		= 13,
	DelayT16		= 14,
	DelayT17		= 15,		
}DELAY_TNO;

void ResetTimer0Relay(DELAY_TNO DelayTxx);
u32bit Timer0SetDelay(DELAY_TNO DelayTxx,u32bit nTick);

#endif //end of __TimeAPI_h__

