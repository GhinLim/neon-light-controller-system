#include "delay.h"
u32 my_us = 21;         	 //1us可计21个数
u32 my_ms = 21000;         //1us可计21000个数

void Delay_Init(void)
{
	//时钟源选择，168MHZ/8 = 21MHZ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //Systick时钟源选择  misc.c文件中

}

//nus取值范围：0~798915
void delay_us(int nus)
{
	u32 temp = 0; 
	
	//设定重载值寄存器
	SysTick->LOAD = my_us*nus - 1;
	//设置计数为0
	SysTick->VAL = 0x00;

	//启动定时器
	SysTick->CTRL |=(0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;  //获取值，判断第16位
			//判断是否使能定时器	//判断第16是否为1	
	}while( (temp & (0x01<<0)) && !(temp & (0x01<<16)));

	//关闭定时器
	SysTick->CTRL &= ~(0x01<<0);
}

//nus取值范围：0~798915
void delay_ms(int nms)
{
	u32 temp = 0; 
	
	//设定重载值寄存器
	SysTick->LOAD = my_ms*nms - 1;
	//设置计数为0
	SysTick->VAL = 0x00;

	//启动定时器
	SysTick->CTRL |=(0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;  //获取值，判断第16位
			//判断是否使能定时器	//判断第16是否为1	
	}while( (temp & (0x01<<0)) && !(temp & (0x01<<16)));

	//关闭定时器
	SysTick->CTRL &= ~(0x01<<0);
}

void delay_s(int ns)
{
	int i;
	for(i=0; i<ns; i++)
	{
		delay_ms(500);
		delay_ms(500);
	}
}
