#include "stm32f4xx.h"
#include "usart1_printf.h"
#include "usart3_BLE.h"
#include "ws2812.h"
#include "display.h"
#include "material.h"
#include "rtc.h"
#define TIME 8 //建议该值>=8

/*********************************************
 * 说明：程序只支持列为16像素的WS2812B LED屏
*********************************************/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Usart1_Init(115200);
	Usart3_Init(9600);
	Delay_Init();
	Rtc_Tim_Init();
	WS2812b_Configuration();
	u8 user_ch[288];
	//LED_Detect2();
	
	while (1)
	{
		delay_ms(1);
		Function();
		//slide(1,BLUE,BLACK,40,0);

		// bufferDetect();
		// delay_ms(500);
		// DisplayFrom_LedColumn(MtrLib[1],BLUE,BLACK,0,0);
		// delay_ms(100);
		
	}
	//rgb_SetColor(118,BLUE);
	
}
