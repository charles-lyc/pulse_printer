#ifndef _HPD482_H
#define _HPD482_H
/*
HPD482驱动程序v3.7 for Arduino UNO
///////////////////////////////////////////////////////////////////////////////////////////////
作者：有立diy
淘宝店铺：有立diy
淘宝链接：https://shop587795831.taobao.com/
原创声明：本例程为热敏打印机模块HPD482配套例程，仅供参考，不得用于商业等用途，其他后果概不负责。
		  如需使用，注明出处。
///////////////////////////////////////////////////////////////////////////////////////////////
*/

#include "Arduino.h"

typedef unsigned char u8;
typedef unsigned short u16;

//定义Arduino uno数据线IO口
#define MNB_pin 	2
#define MB_pin		3
#define MNA_pin		4
#define MA_pin		5
#define LAT_pin		6
#define DI_pin		7
#define CLK_pin		8
#define STBA_pin	9
#define STBB_pin	10

//正序
// #define MNB_pin 	2
// #define MB_pin		3
// #define MNA_pin		4
// #define MA_pin		5
// #define LAT_pin		6
// #define DI_pin		7
// #define CLK_pin		8
// #define STBA_pin	9
// #define STBB_pin	10


#define MNB(pinlevel)	digitalWrite(MNB_pin,pinlevel)//步进电机数据线
#define MB(pinlevel) 	digitalWrite(MB_pin,pinlevel)//步进电机数据线
#define MNA(pinlevel) 	digitalWrite(MNA_pin,pinlevel)//步进电机数据线
#define MA(pinlevel) 	digitalWrite(MA_pin,pinlevel)//步进电机数据线
#define LAT(pinlevel) 	digitalWrite(LAT_pin,pinlevel)//锁存
#define DI(pinlevel) 	digitalWrite(DI_pin,pinlevel)//数据输入
#define CLK(pinlevel) 	digitalWrite(CLK_pin,pinlevel)//时钟
#define STBA(pinlevel) 	digitalWrite(STBA_pin,pinlevel)//加热线A
#define STBB(pinlevel) 	digitalWrite(STBB_pin,pinlevel)//加热线B

//是否打印黑白图片（0：否，1：是）
#define IS_PRINT_PICT	1
//是否打印ASCII字符串（0：否，1：是）
#define IS_PRINT_ASCII	1
//是否打印12号ASCII字符（0：否，1：是）
#define IS_ASCII12_EN 	1
//是否打印16号ASCII字符（0：否，1：是）
#define IS_ASCII16_EN 	1
//是否打印24号ASCII字符（0：否，1：是）
#define IS_ASCII24_EN 	1
#define IS_ASCII40_EN 	1

class HPD482
{
private:
	u16 Ptdeepcolor=0;//打印颜色深度参数
	u16 Leftdeep=0;//左半部分颜色深度参数
	u8 Send_OneByte(u8 onebytedata);
	void Print_Control(u16 tim,u8 ctr);
public:
	u16 Motorspeed=2000;//步进电机转速参数
	u8 motor_step_i=0;//步进电机序列号
	void pdelay_us(u16 tus);
	void pdelay_ms(u16 tms);
	void Print_OneLine(u8 *pixlindat);
	HPD482();
	void Print_Clean(void);
	void Motor_Run(u8 cont,u8 dir);
	void Motor_Back(u16 dot);
	void Print_SetLfDeep(u8 leftdeepcolor);
	void Print_SetDeep(u8 deepcolor);
	void Print_Stop(void);
	void Print_Lines(u8 linesnum,u8 dist);
#if IS_PRINT_PICT
	void Print_Picture(u8 startx,u8 *picdata,u16 widthpix,u16 heightpix,u8 InEx);
	void Print_Img2Lcd(u8 x,u8 *pic,u8 InEx);
#endif
#if IS_PRINT_ASCII
	void Print_String(u8 *onestr,u8 strnum,u8 sizeh,u8 sizew);
	void Print_SStr(u8 *mulstr,u8 sizeh,u8 sizew);
#endif
};

//加热时间常数定义，方程y=14x+600
#define HT_mindots		30		//最少点数
#define HT_maxdots		100		//最多点数
#define HT_minhttim		1020	//最短加热时间
#define HT_maxhttim		2000	//最长加热时间
#define HT_kdottim		14		//点数-时间 斜率
#define HT_bdottim		600		//点数-时间 截距
//减速延时/ms
#define SLOW_DELAY		0u
//对齐延时/us
#define ALIGN_DELAY		4600u//arduino uno

//步进电机序列
#define MOTOR_STEP1		MA(LOW);MNA(HIGH);MB(LOW);MNB(HIGH);//STEP1
#define MOTOR_STEP2		MA(LOW);MNA(HIGH);MB(HIGH);MNB(LOW);//STEP2
#define MOTOR_STEP3		MA(HIGH);MNA(LOW);MB(HIGH);MNB(LOW);//STEP3
#define MOTOR_STEP4		MA(HIGH);MNA(LOW);MB(LOW);MNB(HIGH);//STEP4
//停转
#define MOTOR_STOP		MA(LOW);MNA(LOW);MB(LOW);MNB(LOW);
//停止加热
#define HEAT_STOP		STBA(LOW);STBB(LOW);


#endif
