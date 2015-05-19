#include "TimerAPI.h"

#define TIMER0_DELAY_COUNT   16

static u32bit volatile	uTimer0DelayCnt = 0;
u32bit volatile			bIsSetTime0Delay = FALSE;
TIMER_RELAY_T volatile Timer0Relay;


static TIMER_DELAY_T	Timer0Delay[TIMER0_DELAY_COUNT];

void Timer0Close(void)
{
	DrvTIMER_ClearTimerEvent(E_TMR0,0);
	DrvTIMER_Close(E_TMR0);
	bIsSetTime0Delay = FALSE;
}
void Timer0CallBack(u32bit data)
{
	u32bit volatile i;
	for(i = 0; i < TIMER0_DELAY_COUNT; i++){
		if(Timer0Delay[i].active == TRUE){
			Timer0Delay[i].curTick--;
			if(Timer0Delay[i].curTick == 0){
				Timer0Relay.TimerRelayArray[i] = 1;
				uTimer0DelayCnt--;
				Timer0Delay[i].active = FALSE;
			}
		}
	}
	if(uTimer0DelayCnt == 0){
		Timer0Close();
	}
}
void Timer0Init(void)
{
	if(bIsSetTime0Delay == FALSE){
		DrvTIMER_Init();									//定时器初始化
		DrvTIMER_Open(E_TMR0,10000,E_PERIODIC_MODE);		//启动定时器0,1ms,周期模式
		DrvTIMER_SetTimerEvent(E_TMR0,10,Timer0CallBack,0);	//装载TMR0事件
		bIsSetTime0Delay = TRUE;
	}
}
u32bit Timer0SetDelay(DELAY_TNO DelayTxx,u32bit nTick)		//ms
{
	if(uTimer0DelayCnt >= TIMER0_DELAY_COUNT)
		return E_DRVTIMER_DELAY_FULL;
	uTimer0DelayCnt++;
	if (Timer0Delay[DelayTxx].active  == FALSE){
		Timer0Delay[DelayTxx].active       = TRUE;
		Timer0Delay[DelayTxx].curTick      = nTick;
		Timer0Relay.TimerRelayArray[DelayTxx] = 0;
	}
	Timer0Init();
	return E_SUCCESS;
}
void ResetTimer0Relay(DELAY_TNO DelayTxx)
{
	Timer0Relay.TimerRelayArray[DelayTxx] 	= 0;
	if(Timer0Delay[DelayTxx].active == TRUE){
		Timer0Delay[DelayTxx].active = FALSE;
		uTimer0DelayCnt--;
	}
}

