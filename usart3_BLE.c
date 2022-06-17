#include "usart3_BLE.h"

u8 buffer[datalen - 3] = {0};
u8 rx_buffer[21] = {0};
u8 ch_buffer[21]={0};
u8 count = 0;

/*****************************************
引脚说明
PB10  ---- USART3_TX(发送端)
PB11  ---- USART3_RX(接收端)

USART3挂在APB1下
******************************************/
void Usart3_Init(int MyBaudRate)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    //串口时钟使能，GPIO 时钟使能。
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    //使能GPIOB时钟：
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // GPIOB10复用映射到USART3 引脚选定复用的功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    // GPIOB11复用映射到USART3 引脚选定复用的功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // GPIO 初始化设置：要设置模式为复用功能。
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //引脚10 11
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;             //复用模式
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;           //推挽式
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;             //上拉
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // 50MHZ
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = MyBaudRate;                                //波特率
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 //全双工模式
    USART_InitStruct.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                     //数据位
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                          //停止位
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件控制流
    //串口参数初始化：设置波特率，字长，奇偶校验等参数。
    USART_Init(USART3, &USART_InitStruct);

    //开启中断并且初始化 NVIC，
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;         //中断通道，中断通道在stm32f4xx.h文件当中查找
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //响应优先级
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //使能通道
                                                           // 3、启定时器中断，配置NVIC。
    NVIC_Init(&NVIC_InitStruct);

    //配置为接收中断（表示有数据过来，CPU要中断进行接收）
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //使能串口。
    USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {

        //清空接受标志位
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

        rx_buffer[count++] = USART_ReceiveData(USART3);
        
        if (count >= 21) //防止越界，count的值最大为20
        {
            count = 0;
            memset(rx_buffer,0,sizeof(rx_buffer));
        }
        //printf("%d\r\n",sizeof(rx_buffer));

        if ((rx_buffer[count - 1] == 0x5A) ) //判断是否到了数据包尾部而且收到是数据包
        {
            printf("recieve package\r\n");
            u8 bytesum=0;
            for(u8 i=0;i<(datalen-3);i++)
            {
                bytesum+=rx_buffer[i+1];
            }
            if(bytesum==rx_buffer[count-2])
            {
                for (u8 i = 0; i < datalen - 3; i++)
                {
                    buffer[i] = rx_buffer[i + 1];
                }
                memset(rx_buffer, 0, sizeof(rx_buffer));
                count = 0;

                rx_flag = 1;
                u8 i = 0;
                cur_switch = buffer[i++]; // 0:开关
                if (buffer[i++] <= (NOM - 1))
                {
                    cur_mtrid = buffer[i - 1];
                    // printf("%d\r\n",cur_mtrid);
                }
                else
                {
                    cur_mtrid = 0; //素材ID
                                   // printf("%d\r\n",cur_mtrid);
                }
                if (buffer[i++] == 0x81)
                {
                    color = BLACK;
                }
                else
                {
                    color = Colourful_Wheel(buffer[i - 1]); //前景色
                }
                if (buffer[i++] == 0x81)
                {
                    BACKGD = BLACK;
                }
                else
                {
                    BACKGD = Colourful_Wheel(buffer[i - 1]); //背景色
                }
                speed = buffer[i++];        //速度（未处理）
                cur_position = buffer[i++]; //位置（未处理）
                GradientFlag = buffer[i++]; //是否渐变
                ch_switch = buffer[i++];    //是否显示用户字符
                if (ch_switch == 1)
                {
                    tim_switch = 0;
                }
                else
                {
                    tim_switch = buffer[i]; //是否显示时间
                }

                if ((cur_switch != 0) && (cur_switch != 1))
                {
                    cur_switch = 1;
                    if (cur_mtrid > 10)
                    {
                        cur_mtrid = 0;
                    }
                }
                printf("switch:%X MtrID:%X color:%d %d %d BACKGD:%d %d %d SPEED:%X Position:%X CH：%X TIM:%X\r\n", cur_switch, cur_mtrid, color.R, color.G, color.B, BACKGD.R, BACKGD.G, BACKGD.B, speed, cur_position, ch_switch, tim_switch);
                rx_flag = 0;
            }
            else
            {
                count=0;
                memset(rx_buffer,0,sizeof(rx_buffer));
            }
            
        }
        else if ((rx_buffer[count - 2] == 0x5C) && (rx_buffer[count-1] ==';')) 
        {

            ch_flag=0;
            //printf("count=%d\r\n",count);
            MtrTotalColumn[NOM+1] = (count -2) * 8; //字符总个数为count-2,记录用户输入素材的总列数
            printf("CH:");
            for(u8 i=0;i<(count-2);i++)
            {
                ch_buffer[i]=rx_buffer[i];
                printf("%c",rx_buffer[i]);
            }
            ch_buffer[20]=count-2;
           
            printf("\r\n");
            count=0;
            memset(rx_buffer,0,sizeof(rx_buffer));
            ch_flag=1;
        }
        
    }
}
