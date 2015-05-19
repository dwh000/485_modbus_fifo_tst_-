#include"UartAPI.h"
#include"IO_Logic.h"
#include "mb.h"
#include "mbutils.h"

#define DATA_OK         0X00
#define TIMEOUT_Err     0x01
#define PE_Err          0x02
#define CONFIGDATA_Err  0x03

u8bit RxBuf[80],TxBuf[60],RxCnt,TxCnt;

u8bit data_in_len;              //数据交换输入数据长度
u8bit dil;                      //用户数据区输入长度
u8bit data_out_len;             //数据交换输出数据长度
u8bit dol;                      //用户数据区输出长度


//uart1用于与微机通信

void Uart1Init(u32bit BaudRate)
{
    STR_UART_T Uart1Param;
    /* UART Setting */
    Uart1Param.u32BaudRate      = BaudRate;                 //波特率115.2K
    Uart1Param.u8cDataBits      = DRVUART_DATABITS_8;       //8位数据位
    Uart1Param.u8cStopBits      = DRVUART_STOPBITS_1;       //1位停止位
    Uart1Param.u8cParity        = DRVUART_PARITY_EVEN;      //偶校验
    Uart1Param.u8cRxTriggerLevel= DRVUART_FIFO_1BYTES;      //1字节数据缓冲区
    /* Select UART Clock Source From 12Mhz*/
    DrvSYS_SelectIPClockSource(E_SYS_UART_CLKSRC,0); 
    DrvUART_Open(UART_PORT1, &Uart1Param);
}


u8bit Uart1Send(u8bit ch)
{  
    while (UART1->FSR.TE_FLAG !=1);
    UART1->DATA = ch;                                       /* Send UART Data from buffer */
    return ch;
}


u8bit Uart1Receive(u8bit *ch)
{

    while(UART1->FSR.RX_EMPTY);

    if(UART1->FSR.PEF){
        UART1->FSR.PEF = 1;                                 //复位奇偶校验标志
        return PE_Err;
    }
    *ch = UART1->DATA;                                      //接收一个字符送串口输入缓冲区
    return DATA_OK;
}


u32bit DataExInit(void)
{
    u8bit cnt;
    u8bit err_code = 0,CheckSum;
    u32bit i,u32BaudRate;

    data_in_len = 2;
    data_out_len = 2;
    S_RTS = 0;                                              //接口板允许接受
    REQ_IT = 1;                                             //接口板请求初始化数据

    //等待接收初始化报文
    for(i=0; i<49; i++){
        err_code = Uart1Receive(&RxBuf[i]);
        if(err_code) break;
    }
    S_RTS = 1;                                              //接口板禁止接收数据

    if(err_code == DATA_OK){
        //计算累加和
        CheckSum=0;
        for (cnt=0;cnt<48;cnt++)    CheckSum=CheckSum+RxBuf[cnt];

        //计数交互数据报文的长度
//        for(cnt = 0; cnt < RxBuf[3];cnt++){
//            if((RxBuf[4+cnt] >= 0x10) && (RxBuf[4+cnt] < 0x20)){
//                data_in_len = data_in_len + RxBuf[4+cnt] - 0x0F;
//            }
//            else if((RxBuf[4+cnt] >= 0x20) && (RxBuf[4+cnt] < 0x30)){
//                data_out_len = data_out_len + RxBuf[4+cnt] - 0x1F;
//            }
//            else err_code = CONFIGDATA_Err;
//        }
        data_in_len     = RxBuf[25];
        data_out_len    = RxBuf[24];
        dil=data_in_len-1;                                  //data_in_len=50,dil=49
        dol=data_out_len-1;                                 //data_out_len=50,dol==49
    }

    //报文的正确性验证
    if(i != 49){                                            //报文长度错误
        TxBuf[1] = 6;
    }
    else if(err_code == PE_Err){                            //奇偶校验错误
        TxBuf[1] = 5;
    }
    else if(CheckSum != RxBuf[48]){                         //校验和错误
        TxBuf[1] = 4;
    }
    else if(RxBuf[0]>126){                                  //站号错误
        TxBuf[1] = 3;
    }
    else if(RxBuf[3]>8){                                    //数据配置长度错误
        TxBuf[1] = 2;
    }
    else if(err_code == CONFIGDATA_Err){
        TxBuf[1] = 1;
    }
    else{
        TxBuf[1] = 0;
    }

    //初始化应答报文生成
    TxBuf[0] = GPC_2;                                       //计算设置的波特率代码
    TxBuf[0] = (TxBuf[0]<<1) + GPC_1;
    TxBuf[0] = (TxBuf[0]<<1) + GPC_0;
    if(TxBuf[1]==0){                                        //生成正确应答报文
        for(i=2;i<48;i++)   TxBuf[i] = 0xaa;
        switch(RxBuf[12]){
            case 0:
                u32BaudRate = 1200;
                break;
            case 1:
                u32BaudRate = 2400;
                break;
            case 2:
                u32BaudRate = 4800;
                break;
            case 3:
                u32BaudRate = 9600;
                break;
            case 4:
                u32BaudRate = 19200;
                break;
            case 5:
                u32BaudRate = 38400;
                break;
            case 6:
                u32BaudRate = 57600;
                break;
            case 7:
                u32BaudRate = 115200;
                break;
            case 8:
                u32BaudRate = 125000;
                break;
            case 9:
                u32BaudRate = 187500;
                break;
            default:
                u32BaudRate = 9600;
                break;
        }
        eMBInit(MB_RTU,RxBuf[0],1,u32BaudRate,(eMBParity)RxBuf[13]);    //初始化接口板
        eMBSetSlaveID(0x01,0xff,"BHYW-DP,CODE:0-01-00",21);
        eMBEnable();
        RDY = 1;                                            //接收初始化数据完毕指示
        REQ_IT = 0;                                         //进入数据交互状态
    }
    else{                                                   //生成错误应答报文
        for(i=2;i<48;i++)   TxBuf[i] = 0x55;
    }
    TxBuf[48]=0;                                            //计算应答报文累加和.
    for(i=0;i<48;i++)   TxBuf[48]=TxBuf[48]+TxBuf[i];

    //发送应答报文
    for(i=0; i<49; i++){
        Uart1Send(TxBuf[i]);
    }
    DelayUs(200);
    //清空发送区域报文
    for(i=0; i<49; i++){
        TxBuf[i] = 0;
    }
    //返回初始化是否完成的信息
    if(TxBuf[1]==0) return E_SUCCESS;
    else    return 1;
}

void DataExRoutine(void)
{
    u8bit cnt;
    u8bit err_code = 0,CheckSum;
    u32bit i;

    S_RTS = 0;                                              //接口板进入数据交换
    REQ_IT = 0;                                             //接口板请求初始化数据

    //等待接收初始化报文
    for(i=0; i<data_in_len; i++){
        err_code = Uart1Receive(&RxBuf[i]);
        if(err_code)
            break;
    }
    S_RTS = 1;                                              //接口板禁止接收数据

    //数据交换报文查错检验
    if(err_code == DATA_OK){
        //计算累加和
        CheckSum=0;
        for (cnt=0;cnt<dil;cnt++)   CheckSum=CheckSum+RxBuf[cnt];
        TxBuf[0] = 0;
    }
    if(err_code == PE_Err){
        TxBuf[0] = 1;
    }
    else if(CheckSum != RxBuf[dil]){
        TxBuf[0] = 2;
    }
    PBF = 1;
    //更新modbus寄存器
    TxBuf[35] &= ~RxBuf[48];                                //操作标志寄存器刷新
    TxBuf[36] &= ~RxBuf[49];                                //操作标志寄存器刷新
    if(RxBuf[47] & BIT3)    TxBuf[34] &= ~BIT3;
    if(RxBuf[48] & BIT7)    TxBuf[35] &= ~BIT7;
    //TxBuf[34] &= ~(RxBuf[47] & (BIT3 | BIT4));
    if(TxBuf[0] == 0){
        //ModbusRegRefrash();
    }

    //计算校验和
    TxBuf[dol]=0;
    for (cnt=0;cnt<dol;cnt++)   TxBuf[dol] += TxBuf[cnt];
    //发送应答数据报文.
    for(i=0; i<data_out_len; i++){
        Uart1Send(TxBuf[i]);
    }
    DelayUs(500);
}



/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1
#define REG_INPUT_NREGS 12
#define REG_HOLDING_START 1
#define REG_HOLDING_NREGS 28
#define REG_COILS_START 1
#define REG_COILS_NREGS 8
#define REG_DISCRETE_START 1
#define REG_DISCRETE_NREGS 48
/* ----------------------- Static variables ---------------------------------*/

static u16bit	usRegInputStart = REG_INPUT_START;
static u16bit	usRegInputBuf[REG_INPUT_NREGS];
static u16bit	usRegHoldingStart = REG_HOLDING_START;
static u16bit	usRegHoldingBuf[REG_HOLDING_NREGS];
static u16bit	usRegCoilsStart = REG_COILS_START;
static u8bit	usRegCoilsBuf[(REG_COILS_NREGS/32+1)*4];
static u16bit	usRegDiscreteStart = REG_DISCRETE_START;
static u8bit	usRegDiscreteBuf[(REG_DISCRETE_NREGS/32+1)*4];

union{
	u8bit u8bitSD;
	struct{
		u8bit SD1:1;
		u8bit SD2:1;
		u8bit SD3:1;
		u8bit SD4:1;
		u8bit SD5:1;
		u8bit SD6:1;
		u8bit SD7:1;
		u8bit SD8:1;
	}SD;
}SetDoneFlag,SetDoneFlag1;

void MBInputRegRead(void);
void MBHoldingRegRead(void);
void MBHoldingRegWrite(void);
void MBCoilsRead(void);
void MBCoilsWrite(void);
void MBDiscreteRead(void);
//输入寄存器刷新回调函数
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if((usAddress >= REG_INPUT_START)
        && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS)){
        MBInputRegRead();
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 ){
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else{
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


//保存寄存器刷新回调函数
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) ){
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode ){
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            MBHoldingRegRead();
            while( usNRegs > 0 ){
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 ){
                switch(iRegIndex){                          //参数更新标识刷新
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        SetDoneFlag.SD.SD3 = 1;             //标定参数需修改.
                        break;
                    case 8:
                    case 9:
                    case 18:
                        SetDoneFlag.SD.SD6 = 1;             //掉包计数器需修改.
                        break;
                    case 10:
                    case 15:
                        SetDoneFlag.SD.SD1 = 1;             //袋重设定值,滤波系数需修改
                        break;
                    case 11:
                    case 19:
                        SetDoneFlag.SD.SD2 = 1;             //粗细流切换值,粗细流开关需修改
                        break;
                    case 12:
                    case 13:
                        SetDoneFlag.SD.SD4 = 1;             //手动落差,袋重净差需修改.
                        break;
                    case 14:
                    case 17:
                    case 21:
                        SetDoneFlag.SD.SD7 = 1;             //袋重修正值,闭环校正开关,抵达时间
                        break;
                    case 16:
                    case 20:
                        SetDoneFlag.SD.SD5 = 1;             //开环追踪,追踪袋数
                        break;
                    case 22:
                    case 23:
                    case 24:
                        SetDoneFlag1.SD.SD1 = 1;            //时间参数已更新
                        break;
				}
				usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
			MBHoldingRegWrite();
		}
	}
	else{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}


//线圈读写刷新回调函数
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

	if( ( usAddress >= REG_COILS_START ) &&
		( usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS ) ){
		iRegIndex = ( int )( usAddress - usRegCoilsStart );
		switch ( eMode ){
			/* Pass current register values to the protocol stack. */
		case MB_REG_READ:
			MBCoilsRead();
			while( usNCoils > 0 ){
				if(usNCoils >= 8){
					*pucRegBuffer++ = xMBUtilGetBits(usRegCoilsBuf,iRegIndex,8);
					iRegIndex += 8;
					usNCoils -= 8;
				}
				else{
					*pucRegBuffer++ = xMBUtilGetBits(usRegCoilsBuf,iRegIndex,usNCoils);
					usNCoils = 0;
				}
			}
			break;

            /* Update current register values with new values from the
             * protocol stack. */
		case MB_REG_WRITE:
			while( usNCoils > 0 ){
				if(usNCoils >= 8){
					xMBUtilSetBits(usRegCoilsBuf,iRegIndex,8,*pucRegBuffer++);
					iRegIndex += 8;
					usNCoils -= 8;
				}
				else{
					xMBUtilSetBits(usRegCoilsBuf,iRegIndex,usNCoils,*pucRegBuffer++);
					usNCoils = 0;
				}
			}
			MBCoilsWrite();
		}
	}
	else{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}
//离散量输入刷新回调函数
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if((usAddress >= REG_INPUT_START)
        && (usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_NREGS)){
        MBDiscreteRead();
        iRegIndex = ( int )( usAddress - usRegDiscreteStart );
        while( usNDiscrete > 0 ){
            if(usNDiscrete >= 8){
                *pucRegBuffer++ = xMBUtilGetBits(usRegDiscreteBuf,iRegIndex,8);
                iRegIndex += 8;
                usNDiscrete -= 8;
            }
            else{
                *pucRegBuffer++ = xMBUtilGetBits(usRegDiscreteBuf,iRegIndex,usNDiscrete);
                usNDiscrete = 0;
            }
        }
    }
    else{
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


//输入寄存器读更新
void MBInputRegRead(void)
{
    *((u32bit *)usRegInputBuf)          = (RxBuf[1]<<24) + (RxBuf[2]<<16)
                                        + (RxBuf[3]<<8) + RxBuf[4];             //未标称变换的净重
    usRegInputBuf[2]                    = (RxBuf[5]<<8) + RxBuf[6];             //上包水泥重
    usRegInputBuf[3]                    = (RxBuf[7]<<8) + RxBuf[8];             //当前时重
    usRegInputBuf[4]                    = *((u16bit *)(RxBuf + 42));            //参数错误报警
    usRegInputBuf[5]                    = *((u16bit *)(RxBuf + 44));            //IO状态
    usRegInputBuf[6]                    = *((u16bit *)(RxBuf + 46)) & 0x00ff;   //IO状态
    usRegInputBuf[7]                    = (RxBuf[27]<<8) + RxBuf[28];           //掉包计数器
    usRegInputBuf[8]                    = (RxBuf[25]<<8) + RxBuf[26];
}
//保持寄存器读更新
void MBHoldingRegRead(void)
{
    *((u32bit *)(usRegHoldingBuf + 0))  = (RxBuf[9]<<24) + (RxBuf[10]<<16)
                                        + (RxBuf[11]<<8) + RxBuf[12];           //传感器量程
    *((u32bit *)(usRegHoldingBuf + 2))  = (RxBuf[13]<<24) + (RxBuf[14]<<16)
                                        + (RxBuf[15]<<8) + RxBuf[16];           //砝码输出值(AD得到)
    *((u32bit *)(usRegHoldingBuf + 4))  = (RxBuf[17]<<24) + (RxBuf[18]<<16)
                                        + (RxBuf[19]<<8) + RxBuf[20];           //砝码值
    *((u32bit *)(usRegHoldingBuf + 6))  = (RxBuf[21]<<24) + (RxBuf[22]<<16)
                                        + (RxBuf[23]<<8) + RxBuf[24];           //皮重
    *((u32bit *)(usRegHoldingBuf + 8))  = (RxBuf[25]<<24) + (RxBuf[26]<<16)
                                        + (RxBuf[27]<<8) + RxBuf[28];           //掉包计数器
    usRegHoldingBuf[10]                 = (RxBuf[29]<<8) + RxBuf[30];           //掉包设定值
    usRegHoldingBuf[11]                 = (RxBuf[31]<<8) + RxBuf[32];           //粗细流切换值
    usRegHoldingBuf[12]                 = (RxBuf[33]<<8) + RxBuf[34];           //落差值
    usRegHoldingBuf[13]                 = (RxBuf[35]<<8) + RxBuf[36];           //净差值
    usRegHoldingBuf[14]                 = (RxBuf[37]<<8) + RxBuf[38];           //袋重修正值
    usRegHoldingBuf[15]                 = RxBuf[39];                            //滤波系数
    usRegHoldingBuf[16]                 = RxBuf[40];                            //跟踪袋数
    usRegHoldingBuf[17]                 = RxBuf[41];                            //抵达时间
    if(RxBuf[47] & BIT4)    usRegHoldingBuf[18] = 1;                            //掉包计数器开关
    else                    usRegHoldingBuf[18] = 0;
    if(RxBuf[47] & BIT5)    usRegHoldingBuf[19] = 1;                            //粗细流切换开关
    else                    usRegHoldingBuf[19] = 0;
    if(RxBuf[47] & BIT6)    usRegHoldingBuf[20] = 1;                            //开环追踪开关
    else                    usRegHoldingBuf[20] = 0;
    if(RxBuf[47] & BIT7)    usRegHoldingBuf[21] = 1;                            //闭环校正开关
    else                    usRegHoldingBuf[21] = 0;
    usRegHoldingBuf[22]                 = RxBuf[50];
    usRegHoldingBuf[23]                 = RxBuf[51];
    usRegHoldingBuf[24]                 = RxBuf[52];
}

//保持寄存器写更新
void MBHoldingRegWrite(void)
{
    TxBuf[1] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 0)) >> 24) & 0XFF);      //传感器量程
    TxBuf[2] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 0)) >> 16) & 0XFF);
    TxBuf[3] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 0)) >> 8) & 0XFF);
    TxBuf[4] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 0)) >> 0) & 0XFF);

    TxBuf[5] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 2)) >> 24) & 0XFF);      //砝码输出值(AD得到)
    TxBuf[6] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 2)) >> 16) & 0XFF);
    TxBuf[7] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 2)) >> 8) & 0XFF);
    TxBuf[8] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 2)) >> 0) & 0XFF);

    TxBuf[9]  = (u8bit)((*((u32bit *)(usRegHoldingBuf + 4)) >> 24) & 0XFF);      //砝码值
    TxBuf[10] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 4)) >> 16) & 0XFF);
    TxBuf[11] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 4)) >> 8) & 0XFF);
    TxBuf[12] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 4)) >> 0) & 0XFF);

    TxBuf[13] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 6)) >> 24) & 0XFF);      //皮重
    TxBuf[14] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 6)) >> 16) & 0XFF);
    TxBuf[15] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 6)) >> 8) & 0XFF);
    TxBuf[16] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 6)) >> 0) & 0XFF);

    TxBuf[17] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 8)) >> 24) & 0XFF);      //掉包计数器
    TxBuf[18] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 8)) >> 16) & 0XFF);
    TxBuf[19] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 8)) >> 8) & 0XFF);
    TxBuf[20] = (u8bit)((*((u32bit *)(usRegHoldingBuf + 8)) >> 0) & 0XFF);

    TxBuf[21] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 10)) >> 8) & 0XFF);
    TxBuf[22] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 10)) >> 0) & 0XFF);

    TxBuf[23] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 11)) >> 8) & 0XFF);
    TxBuf[24] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 11)) >> 0) & 0XFF);

    TxBuf[25] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 12)) >> 8) & 0XFF);
    TxBuf[26] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 12)) >> 0) & 0XFF);

    TxBuf[27] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 13)) >> 8) & 0XFF);
    TxBuf[28] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 13)) >> 0) & 0XFF);

    TxBuf[29] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 14)) >> 8) & 0XFF);
    TxBuf[30] = (u8bit)((*((u16bit *)(usRegHoldingBuf + 14)) >> 0) & 0XFF);

    TxBuf[31] = (u8bit)(usRegHoldingBuf[15] & 0XFF);

    TxBuf[32] = (u8bit)(usRegHoldingBuf[16] & 0XFF);

    TxBuf[33] = (u8bit)(usRegHoldingBuf[17] & 0XFF);

    if(usRegHoldingBuf[18])      TxBuf[34] |= (u8bit)BIT4;
    else      TxBuf[34] &= ~(u8bit)BIT4;
    if(usRegHoldingBuf[19])      TxBuf[34] |= (u8bit)BIT5;
    else      TxBuf[34] &= ~(u8bit)BIT5;
    if(usRegHoldingBuf[20])      TxBuf[34] |= (u8bit)BIT6;
    else      TxBuf[34] &= ~(u8bit)BIT6;
    if(usRegHoldingBuf[21])      TxBuf[34] |= (u8bit)BIT7;
    else      TxBuf[34] &= ~(u8bit)BIT7;

    TxBuf[35] = SetDoneFlag.u8bitSD;
    TxBuf[36] = SetDoneFlag1.u8bitSD;
    TxBuf[37] = (u8bit)(usRegHoldingBuf[22] & 0XFF);
    TxBuf[38] = (u8bit)(usRegHoldingBuf[23] & 0XFF);
    TxBuf[39] = (u8bit)(usRegHoldingBuf[24] & 0XFF);
    
}
//线圈读更新
void MBCoilsRead(void)
{
    usRegCoilsBuf[0] = (RxBuf[47] & 0x0f) | ((RxBuf[48]>>3) & BIT4);
}
//线圈写更新
void MBCoilsWrite(void)
{
    TxBuf[34] = usRegCoilsBuf[0] & 0X0F;
    TxBuf[35] |= ((usRegCoilsBuf[0] & BIT4)<<3);
}
//离散量输入读更新
void MBDiscreteRead(void)
{
	usRegDiscreteBuf[0] = RxBuf[43];
	usRegDiscreteBuf[1] = RxBuf[42];
	usRegDiscreteBuf[2] = RxBuf[45];
	usRegDiscreteBuf[3] = RxBuf[44];
	usRegDiscreteBuf[4] = RxBuf[46];
	usRegDiscreteBuf[5] = 0;
}

