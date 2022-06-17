#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "stm32f4xx.h"
#include "ws2812.h"
#include "delay.h"
#include "string.h"
#include "material.h"
#include "rtc.h"
#include "usart3_BLE.h"

extern RGBColor_TypeDef color;
extern RGBColor_TypeDef BACKGD;

extern u8 cur_switch;
extern u8 pre_switch;
extern u8 ch_switch;
extern u8 tim_switch;
extern u8 cur_mtrid;
extern u8 pre_mtrid;
extern u8 pre_clock;
extern u8 cur_clock;
extern u8 speed;
extern u8 column;
extern u8 CH_column;   //用于记录用户字符：列
extern u8 totalcoloum;
extern u8 Gradient;
extern u8 GradientFlag;
extern u8 pre_position;
extern u8 cur_position;
extern u8 rx_flag;
extern u8 ch_flag;

void MCON(void);
void display(void);
void display2(void);
void Function(void);
void CH_display(void);
void TIM_display(void);
void MTR_display(void);
void slide(u8 mtrid, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 speed,u8 GradientFlag);
void DisplayFrom_LedColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag);
void DisplayFrom_MtrColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag);
u16 ParseAndLoad(u8 mtrx[], u16 StartMtrId, u16 EndMtrId, u16 LedId, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 flag, u8 GradientFlag);

#endif // !__DISPLAY_H

