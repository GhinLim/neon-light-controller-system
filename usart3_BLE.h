#ifndef __USART3BLE_H
#define __USART3BLE_H

#include "stm32f4xx.h"
#include "ws2812.h"
#include "display.h"

#define datalen 12
extern u8 buffer[];
extern u8 rx_buffer[];
extern u8 ch_buffer[];
extern u8 count;


void Usart3_Init(int MyBaudRate);

#endif // !__usart3_BLE_H