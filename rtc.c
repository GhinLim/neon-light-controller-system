#include "rtc.h"
#define BKP_DR 0x0004

u16 TmpSpeed;
u8 TmpGradientFlag;
RGBColor_TypeDef Tmpcolor, TmpBACKGD;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
void Rtc_Tim_Init(void)
{

	RTC_InitTypeDef RTC_InitStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	// 1、使能PWR时钟,与电源寄存器有关
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	// 2、使能后备寄存器访问，是一种掉电保存数据的寄存器
	PWR_BackupAccessCmd(ENABLE);

	//判断后备寄存器的值
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_DR)
	{
		// 3、配置RTC时钟源，使能RTC时钟：
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//如果使用LSE，要打开LSE：
		RCC_LSEConfig(RCC_LSE_ON);
		//延时一段时间，等待时钟源稳定
		delay_ms(50);

		RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24; // 24小时制
		RTC_InitStruct.RTC_AsynchPrediv = 0x7F;			   //异步分频 128分频
		RTC_InitStruct.RTC_SynchPrediv = 0xFF;			   //同频分频 255分频
		// 4、初始化RTC(同步/异步分频系数和时钟格式)：
		RTC_Init(&RTC_InitStruct);

		RTC_TimeStruct.RTC_H12 = RTC_H12_PM; //下午 24小时制，可写可不写
		RTC_TimeStruct.RTC_Hours = 21;		 //小时
		RTC_TimeStruct.RTC_Minutes = 1;		 //分
		RTC_TimeStruct.RTC_Seconds = 50;	 // ADC+
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

		RTC_DateStruct.RTC_Year = 22;					 //年份
		RTC_DateStruct.RTC_Month = 5;					 //月份
		RTC_DateStruct.RTC_Date = 16;					 //日
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Monday; //星期
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

		//将值写入后备寄存器
		RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_DR);
	}
}

//闹钟A初始化
void RTC_Alarm_AInit(u8 data, u8 hours, u8 minutes, u8 seconds)
{
	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_TimeTypeDef RTC_AlarmTime;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	//关闭闹钟：
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	RTC_AlarmTime.RTC_H12 = RTC_H12_PM;	 //下午 24小时制，可写可不写
	RTC_AlarmTime.RTC_Hours = hours;	 //小时
	RTC_AlarmTime.RTC_Minutes = minutes; //分
	RTC_AlarmTime.RTC_Seconds = seconds; // ADC+

	RTC_AlarmStruct.RTC_AlarmTime = RTC_AlarmTime;							//设置时间
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;						//无掩码，按实际设置的时间响应闹钟
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; //按日期响应闹钟
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = data;							// 16号
	//配置闹钟参数：
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	//开启闹钟：
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	//开启配置闹钟中断：
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	EXTI_InitStruct.EXTI_Line = EXTI_Line17;			//中断线17
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;				//中断线使能
														//初始化线上中断，设置触发条件等。
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;	   //中断通道，中断通道在stm32f4xx.h文件当中查找
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;		   //响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;		   //使能通道
														   //配置中断分组（NVIC），并使能中断。
	NVIC_Init(&NVIC_InitStruct);

	// TmpSpeed = speed;
	// TmpGradientFlag = GradientFlag;
	// Tmpcolor=color, TmpBACKGD=BACKGD;

	// speed=0;
	// GradientFlag=0;
	// color=RED;
	// BACKGD=BLACK;
}

//闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line17) == SET)
	{
		//判断是否为闹钟A
		if (RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
		{
			AlarmLight();
			// speed=TmpSpeed;
			// GradientFlag=TmpGradientFlag;
			// color=Tmpcolor;
			// BACKGD=TmpBACKGD;
		}
		RTC_ClearFlag(RTC_FLAG_ALRAF);
	}
	//清空标志位
	EXTI_ClearITPendingBit(EXTI_Line17);
}
