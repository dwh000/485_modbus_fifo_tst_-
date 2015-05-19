/*
 * FreeModbus Libary: nuc140 Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
//static void Timer1CallBack(u32bit data);

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
//    DrvTIMER_Init();
//    DrvTIMER_Open(E_TMR1,8000/usTim1Timerout50us,E_ONESHOT_MODE);//启动定时器,定时为T3.5*2.5
//    TMR1_EnableInt(Timer1CallBack);                         //装载TMR1中断
//    TIMER1->TCSR.CEN = 0;
//    TIMER1->TDR = 0;                                // Disable Timer and Reset Counter
//    TIMER1->TISR.TIF = 1;                           //确保复位中断标志位
//    DrvTIMER_EnableInt(E_TMR1);
    UART0->TOR.TOIC = usTim1Timerout50us;
    UART0->IER.RTO_IEN = 1;
    UART0->IER.TIME_OUT_EN = 1;
    return TRUE;
}


void
vMBPortTimersEnable(  )
{
//    TIMER1->TCSR.CEN = 0;
//    TIMER1->TDR = 0;                                        // Disable Timer and Reset Counter
//    TIMER1->TCSR.CEN = 1;                                   // Enable Timer
//    UART0->IER.TIME_OUT_EN = 1;
}

void
vMBPortTimersDisable(  )
{
//    TIMER1->TCSR.CEN = 0;
//    TIMER1->TDR = 0;                                        // Disable Timer and Reset Counter
//    UART0->IER.TIME_OUT_EN = 0;
}

//static void Timer1CallBack(u32bit data)
//{
//    (void)pxMBPortCBTimerExpired();
//    //GPA_11 = !GPA_11;                                     //定时周期测试!
//}

