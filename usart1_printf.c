#include "usart1_printf.h"

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数   printf 是一个宏
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //通过串口发送数据
	//等待数据发送完毕
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}


/*****************************************
引脚说明
PA9  ---- USART1_TX(发送端)
PA10 ---- USART1_RX(接收端)
******************************************/
void Usart1_Init(int MyBaudRate)
{
	
	GPIO_InitTypeDef  	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//串口时钟使能，GPIO 时钟使能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //使能GPIOA时钟：
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//GPIOA9复用映射到USART1 引脚选定复用的功能
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 	
	//GPIOA10复用映射到USART1 引脚选定复用的功能
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 	
	
	//GPIO 初始化设置：要设置模式为复用功能。
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9|GPIO_Pin_10;		//引脚9 10
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;		//复用模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;	//推挽式
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//上拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOA,&GPIO_InitStruct);	


	USART_InitStruct.USART_BaudRate				= MyBaudRate;					//波特率
	USART_InitStruct.USART_Mode					= USART_Mode_Tx|USART_Mode_Rx;	//全双工模式
	USART_InitStruct.USART_Parity				= USART_Parity_No;				//无奇偶校验位
	USART_InitStruct.USART_WordLength			= USART_WordLength_8b;			//数据位
	USART_InitStruct.USART_StopBits				= USART_StopBits_1;				//停止位
	USART_InitStruct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;//无硬件控制流
	//串口参数初始化：设置波特率，字长，奇偶校验等参数。
	USART_Init(USART1, &USART_InitStruct);
	
	//开启中断并且初始化 NVIC，
	NVIC_InitStruct.NVIC_IRQChannel						= USART1_IRQn; //中断通道，中断通道在stm32f4xx.h文件当中查找
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;		 //抢占优先级			
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 0;		 //响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;	 //使能通道
	//3、启定时器中断，配置NVIC。
    NVIC_Init(&NVIC_InitStruct);		


	//配置为接收中断（表示有数据过来，CPU要中断进行接收）
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
	
	
	//使能串口。
	USART_Cmd(USART1, ENABLE);
	printf("usart1 activated!\r\n");

}
