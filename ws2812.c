#include "WS2812.h"

// Some Static Colors
const RGBColor_TypeDef RED      = {255,0,0};
const RGBColor_TypeDef GREEN    = {0,255,0};
const RGBColor_TypeDef BLUE     = {0,0,255};
const RGBColor_TypeDef SKY      = {0,255,255};
const RGBColor_TypeDef MAGENTA  = {255,0,255};
const RGBColor_TypeDef YELLOW   = {255,255,0};
const RGBColor_TypeDef ORANGE   = {127,106,0};
const RGBColor_TypeDef BLACK    = {0,0,0};
const RGBColor_TypeDef WHITE    = {255,255,255};
const RGBColor_TypeDef PURPLE   = {65,105,225};

u8 pixelBuffer[Pixel_S1_NUM][24];                     //灯珠

/********************************
 *逐个打印出每个像素点的RGB值
********************************/
void detect(void)
{
  int color=0;
  for(u16 i=0;i<Pixel_S1_NUM;i++)
  {
    //打印出每个灯的ID
    printf("[%d]",i);

    //Color.G,第0个数是G的最高位
    for(u8 j=0;j<8;j++)
    {
      //1码
      if(pixelBuffer[i][j]==CODE1)
      {
        color|=1<<(15-j);
      }
    }
    //Color.R
    for(u8 j=8;j<16;j++)
    {
      //1码
      if(pixelBuffer[i][j]==CODE1)
      {
        color|=1<<(23-(j-8));
      }
    
    }
    //Color.B
    for(u8 j=16;j<24;j++)
    {
      //1码
      if(pixelBuffer[i][j]==CODE1)
      {
        color|=1<<(7-(j-16));
      }
      //0码
    }
    printf("%#X ",color);
    color=0;
  }
  printf("\r\nPrint Completed!\r\n");
  
}
/***********************************************************************************************
**     name: WS2812b_Configuration
** function:  WS2812B SPI DMA总线初始化
**parameter: void
**   return: void
************************************************************************************************/
void WS2812b_Configuration(void){
	
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //使能SPI2时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);   //DMA1时钟使能 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                    //PB15复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);                        //初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);        //PB15复用为 SPI2

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//42M/8=5.25M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);                                   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE);                                                //使能SPI外设
  
	DMA_DeInit(DMA1_Stream4);

	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}                       //等待DMA可配置 
		
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;                            //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;                //DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32) & (*pixelBuffer);           // DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                   //存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = Pixel_S1_NUM * 24;                     //数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                     //中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;               //存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;       //外设突发单次传输
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);                               //初始化DMA Stream
  
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // 使能SPI2的DMA发送 	
	DMA_Cmd(DMA1_Stream4, DISABLE);                            //关闭DMA传输 	
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	       //确保DMA可以被设置  		
	DMA_SetCurrDataCounter(DMA1_Stream4,Pixel_S1_NUM * 24);    //数据传输量  
	DMA_Cmd(DMA1_Stream4, ENABLE); 
  
  delay_ms(20);
  
  for(u8 i=0;i<TIME;i++)
  {
    RGB_BLACK(Pixel_S1_NUM);    
  }
                          
  delay_ms(1);
  printf("Activation completed!\r\n");

}

void LED_Detect(void)
{
  for(int i=0;i<Pixel_S1_NUM;i++)
  {
    rgb_SetColor(i,Colourful_Wheel(i));
    rgb_SendArray();
    delay_ms(100);
  }
}

void LED_Detect2(void)
{
  for(int i=0;i<256;i++)
  {
    for(int j=0;j<Pixel_S1_NUM;j++)
    {
      rgb_SetColor(j,Colourful_Wheel(i));
    }
    rgb_SendArray();
    delay_ms(100);
  }
}

void LED_Detect3(void)
{
  for(int i=0;i<Pixel_S1_NUM;i++)
  {
    for(int j=0;j<=i;j++)
    {
      RGB_BLACK(j);
    }
    for(int k=0;k<256;k++)
    {
      rgb_SetColor(i,Colourful_Wheel(k));
      rgb_SendArray();
      delay_ms(1);
    }
  }
}

/***********************************************************************************************
**     name: rgb_SetColor
** function: 设定某个RGB LED的颜色
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color){
	
 	u16 i;
	
  if( LedId > ( Pixel_S1_NUM ) ){
    printf("Error:Out of Range!\r\n");
		return;                               //to avoid overflow
	}
  
  for(i=0;i<=7;i++){
		pixelBuffer[LedId][i]= ( (Color.G & (1 << (7 -i)) )? (CODE1):CODE0 );
	}
  for(i=8;i<=15;i++){
		pixelBuffer[LedId][i]= ( (Color.R & (1 << (15-i)) )? (CODE1):CODE0 );
	}
  for(i=16;i<=23;i++){
		pixelBuffer[LedId][i]= ( (Color.B & (1 << (23-i)) )? (CODE1):CODE0 );
	}
}

/***********************************************************************************************
**     name: rgb_SendArray
** function: Configure colors to RGB pixel series.
             RGBColor_TypeDef: pointer to a RGBColor_TypeDef structure that contains the color configuration information for the RGB pixel.
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SendArray(void){
	
  // while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4) != RESET)
  // {
  //   printf("remain %d\r\n",DMA_GetCurrDataCounter(DMA1_Stream4));
  //   DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
  // }
	if(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4) != RESET){ //等待DMA1_Steam5传输完成			
		DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);                //清除DMA1_Steam5传输完成标志，先预想DMA_FLAG_TCIF0的零，代表的是Channel		
	 	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // 使能SPI3的DMA发送 	
		DMA_Cmd(DMA1_Stream4, DISABLE);                            //关闭DMA传输 	
		while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	       //确保DMA可以被设置  		
		DMA_SetCurrDataCounter(DMA1_Stream4,Pixel_S1_NUM * 24);    //数据传输量  
		DMA_Cmd(DMA1_Stream4, ENABLE);                             //开启DMA传输 
  }

}

/***********************************************************************************************
**     name: RGB_RED
** function: 设定颜色为RED
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_RED(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){  
    rgb_SetColor(i,RED);
	}
  rgb_SendArray();
	

}

/***********************************************************************************************
**     name: RGB_PURPLE
** function: 设定颜色为PURPLE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_PURPLE(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){  
    rgb_SetColor(i,PURPLE);
	}
  
  rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_SKY
** function: 设定颜色为SKY
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_SKY(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){  
    rgb_SetColor(i,SKY);
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_MAGENTA
** function: 设定颜色为MAGENTA
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_MAGENTA(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){  
    rgb_SetColor(i,MAGENTA);
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_ORANGE
** function: 设定颜色为ORANGE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_ORANGE(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){  
    rgb_SetColor(i,ORANGE);
	} 
	
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_GREEN
** function: 设定颜色为GREEN
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_GREEN(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){
    rgb_SetColor(i,GREEN);
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_BLUE
** function: 设定颜色为BLUE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLUE(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){
    rgb_SetColor(i,BLUE);
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_YELLOW
** function: 设定颜色为YELLOW
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_YELLOW(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){
    rgb_SetColor(i,YELLOW);
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_BLACK
** function: 设定颜色为all-off
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLACK(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){
  
    rgb_SetColor(i,BLACK);
	}

  rgb_SendArray();
  
}

/***********************************************************************************************
**     name: RGB_WHITE
** function: 设定颜色为WHITE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_WHITE(u16 Pixel_LEN){
	
  u16 i;
	
  for(i = 0; i < Pixel_LEN; i++){
    rgb_SetColor(i,WHITE);
    
	}
  rgb_SendArray();
}

/***********************************************************************************************
**     name: Colourful_Wheel
** function: 将颜色转换为GRB
**parameter: WheelPos 颜色数值
**   return: RGBColor_TypeDef 颜色GRB
************************************************************************************************/
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos){
	
	RGBColor_TypeDef color;
  WheelPos = 255 - WheelPos;
  
  if(WheelPos < 85){
    color.R = 255 - WheelPos * 3;
    color.G = 0;
    color.B = WheelPos * 3;
		return color;
  }
  if(WheelPos < 170){
    WheelPos -= 85;
    color.R = 0;
    color.G = WheelPos * 3;
    color.B = 255 - WheelPos * 3;
		return color;
  }
  
  WheelPos -= 170;
  color.R = WheelPos * 3; 
  color.G = 255 - WheelPos * 3;
  color.B = 0;
  
  return color;  
}

/***********************************************************************************************
**     name: rainbowCycle
** function: 呼吸灯功能
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void rainbowCycle(u16 Pixel_LEN){
	
  u16 i, j = 0;

  for(j = 0; j < 1024; j++){                                                   // 1 cycles of all colors on wheel
    for(i = 0; i < Pixel_LEN; i++){  
      rgb_SetColor(i,Colourful_Wheel(((i * 256 / Pixel_LEN) + j)&255));
		} 
    rgb_SendArray();
    delay_ms(20);
  }
}

