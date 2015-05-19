// Header:
// File Name: 
// Author:
// Date:
#include <stdio.h>
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "IO_logic.h"
#include "UartAPI.h"
#include "mbport.h"
#include "mb.h"

void ClkInit(void);
/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/    

int main (void)
{
    ClkInit();
    GPIOInit();
    Uart1Init(GetUart1Baud());
    while(DataExInit());
    while(1){
        //printf("你好,新唐开发者\n");
        DataExRoutine();
        eMBPoll();
    }
}



void ClkInit(void)
{
    UNLOCKREG();

    /* Enable 12M Crystal */
    DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);
    DrvSYS_Delay(5000);
    /* Enable PLL */
    DrvSYS_SetPLLMode(0);

    /* Switch to PLL clock */
    DrvSYS_SelectHCLKSource(2);
}








