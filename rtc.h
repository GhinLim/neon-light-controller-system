#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx.h"
#include "delay.h"
#include "ws2812.h"
#include "display.h"
#include "usart3_BLE.h"

extern u16 TmpSpeed;
extern u8 TmpGradientFlag;
extern RGBColor_TypeDef Tmpcolor, TmpBACKGD;
extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;

void Rtc_Tim_Init(void);
void RTC_Alarm_AInit(u8 data, u8 hours, u8 minutes, u8 seconds);
void RTC_Alarm_BInit(void);
#endif