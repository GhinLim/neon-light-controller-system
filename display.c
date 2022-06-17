#include "display.h"
RGBColor_TypeDef color;
RGBColor_TypeDef BACKGD;

u8 cur_switch = 0;
u8 pre_switch = 0;
u8 ch_switch=0;
u8 tim_switch=0;
u8 cur_mtrid;
u8 pre_mtrid = 255;
u8 pre_clock=1;
u8 cur_clock=0;
u8 speed;
u8 column;    //用于记录素材：列
u8 CH_column; //用于记录用户字符：列
u8 totalcoloum;
u8 Gradient = 0;
u8 GradientFlag = 0;
u8 pre_position = 0;
u8 cur_position;
u8 rx_flag = 0;
u8 ch_flag = 0;
u8 time_flag = 0;
static u8 user_ch[288];

//从LED屏的第column列开始显示素材，column最小值为0
void DisplayFrom_LedColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag)
{
    //列数不能大于屏幕总列数
    if (column > (Pixel_S1_NUM / 16 - 1))
    {
        printf("Out of LedDColumn Range!\r\n");
        return;
    }

    u16 LedId;
    u16 BACKGDcount = column * 16; //不显示素材的LED个数
    //把前面不显示素材的LED列置为背景色
    for (LedId = 0; LedId < BACKGDcount; LedId++)
    {
        rgb_SetColor(LedId, BACKGD);
    }

    //计算总共有多少个元素
    u16 size = MtrTotalColumn[cur_mtrid] * 2;

    //计算将要显示的素材量
    u16 EndMtrId;
    EndMtrId = (size < (Pixel_S1_NUM - BACKGDcount) / 8) ? size : ((Pixel_S1_NUM - BACKGDcount) / 8);

    // flag：0代表正向，1代表反向
    u8 flag = (column % 2 == 0) ? 0 : 1;
    //解析并装载
    LedId = ParseAndLoad(mtrx, 0, EndMtrId, LedId, color, BACKGD, flag, GradientFlag);

    //把剩余的LED填充为背景色
    for (u16 i = LedId; i < Pixel_S1_NUM; i++)
    {
        rgb_SetColor(i, BACKGD);
    }

    //开始数据传输
    rgb_SendArray();
}

//从LED屏的第0列开始，从素材的第column列开始显示，column的最小值为0
void DisplayFrom_MtrColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag)
{
    //计算素材总共有多少个元素
    u16 size = MtrTotalColumn[cur_mtrid] * 2;

    //计算从第几个元素开始解析
    u16 StartMtrId = column * 2; //每一列包含2个素材元素
    if (StartMtrId > size)
    {
        printf("MtrColumn Out of Range!\r\n");
        return;
    }

    //计算剩余元素个数
    u16 RemainMtrCount = size - StartMtrId;

    //求出结束元素
    u16 EndMtrId = (RemainMtrCount * 8 >= Pixel_S1_NUM) ? (StartMtrId + Pixel_S1_NUM / 8) : (StartMtrId + RemainMtrCount);

    //装载并解析
    u16 LedId = 0;
    LedId = ParseAndLoad(mtrx, StartMtrId, EndMtrId, LedId, color, BACKGD, 0, GradientFlag);

    //把剩余的LED填充为背景色
    for (u16 i = LedId; i < Pixel_S1_NUM; i++)
    {
        rgb_SetColor(i, BACKGD);
    }
    rgb_SendArray();
}

//解析并装载
u16 ParseAndLoad(u8 mtrx[], u16 StartMtrId, u16 EndMtrId, u16 LedId, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 flag, u8 GradientFlag)
{
    for (u16 i = StartMtrId; i < EndMtrId; i = i + 2)
    {
        //正    ((i * 256 / Pixel_LEN) + j)&255)
        if (flag == 0)
        {
            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i] & (0x01 << j))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i + 1] & (0x01 << j))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }
        }
        //反
        else
        {

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i + 1] & (0x01 << (7 - j)))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i] & (0x01 << (7 - j)))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }
        }
        flag = !flag;
    }
    return LedId;
}

void slide(u8 mtrid, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 speed, u8 GradientFlag)
{
    cur_mtrid = mtrid;
    for (u8 i = Pixel_S1_NUM / 16 - 1; i > 0; i--)
    {
        DisplayFrom_LedColumn(MtrLib[cur_mtrid], color, BACKGD, i, GradientFlag);
        delay_ms(speed);
    }
    for (u8 i = 0; i < MtrTotalColumn[cur_mtrid]; i++)
    {
        DisplayFrom_MtrColumn(MtrLib[cur_mtrid], color, BACKGD, i, GradientFlag);
        delay_ms(speed);
    }
}

//报警灯效果
void AlarmLight(void)
{
    u8 flag = 0, time = 35;
    RGBColor_TypeDef color;
    u16 i, j, k, n, value1, value2;

    //清屏
    for (n = 0; n < TIME; n++)
    {
        RGB_BLACK(Pixel_S1_NUM);
        rgb_SendArray();
    }
    //闪烁
    for (n = 0; n < 4; n++)
    {
        value1 = (flag == 0) ? 0 : Pixel_S1_NUM / 2;
        value2 = (flag == 0) ? Pixel_S1_NUM / 2 : Pixel_S1_NUM;
        for (i = 0; i < 10; i++)
        {
            color = (i % 2 == 1) ? BLACK : ((flag == 0) ? RED : BLUE);
            for (k = 0; k < TIME; k++)
            {
                for (j = value1; j < value2; j++)
                {
                    rgb_SetColor(j, color);
                }
                rgb_SendArray();
            }
            delay_ms(time);
        }
        flag = !flag;
        delay_ms(1500);
    }
}

void display(void) //用于显示普通素材
{
    if (column < (Pixel_S1_NUM / 16))
    {
        // printf("进场模式\r\n");
        //进场模式
        column = Pixel_S1_NUM / 16 - column - 1; //从最后一列开始进场
        DisplayFrom_LedColumn(MtrLib[cur_mtrid], color, BACKGD, column, GradientFlag);
    }
    else
    {
        //出场模式
        column -= Pixel_S1_NUM / 16; //从0列开始出场
        DisplayFrom_MtrColumn(MtrLib[cur_mtrid], color, BACKGD, column, GradientFlag);
    }
}

void display2(void)
{

    if (CH_column < (Pixel_S1_NUM / 16))
    {
        CH_column = Pixel_S1_NUM / 16 - CH_column - 1; //从最后一列开始进场
        DisplayFrom_LedColumn(user_ch, color, BACKGD, CH_column, GradientFlag);
    }
    else
    {
        CH_column -= Pixel_S1_NUM / 16; //从0列开始出场
        DisplayFrom_MtrColumn(user_ch, color, BACKGD, CH_column, GradientFlag);
    }
}

void MCON(void) //素材是否发生改变
{
    if (cur_mtrid != pre_mtrid) //素材发生了改变
    {
        pre_mtrid = cur_mtrid;
        cur_position = (1 - (float)Pixel_S1_NUM / 16 / (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8)) * 127; //位置移动到素材首格
    }
    column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
}

/*****************************************************************************
 * 0      1      2          3         4        5        6       7
 * 开关   素材    前景色     背景色    速度      位置     渐变     字符
 *
 *
 ****************************************************************************/

void CH_display(void) //显示用户输入字符
{
    
    if (speed == 0)
    {
        if(cur_position != pre_position)
        {
            CH_column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
        }
        
        display2();
    }
    else
    {
        if (cur_position != pre_position) //位置发生了改变
        {
            pre_position = cur_position;
            CH_column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);

            display2();
        }
        else
        {
            CH_column++;
            if (CH_column >= (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 16))
            {
                CH_column = 0;
            }
            u8 tmp_column = CH_column;
            display2();
            CH_column = tmp_column;
        }
        delay_ms(120 - speed);
    }
    
}

void MTR_display(void) //显示素材
{
    if (speed == 0) //静止
    {
        MCON();
        display();
    }
    else //滑动
    {
        if (cur_position != pre_position) //位置发生了改变
        {
            pre_position = cur_position;
            column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
            MCON();
            display();
        }
        else
        {

            column++;
            if (column >= (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 16))
            {
                column = 0;
            }
            u8 tmp_column = column;
            display();
            column = tmp_column;
        }
        delay_ms(120 - speed);
    }
}


void TIM_display(void)
{
    u8 time_buffer[8];
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

    time_buffer[0] = RTC_TimeStruct.RTC_Hours / 10 + '0'; //时
    time_buffer[1] = RTC_TimeStruct.RTC_Hours % 10 + '0';
    
    if(RTC_TimeStruct.RTC_Seconds % 2==0)
    {
        time_buffer[2] = ':';
        time_buffer[5] = ':';
    }
    else
    {
        time_buffer[2]=' ';
        time_buffer[5] = ' ';
    }
    
    time_buffer[3] = RTC_TimeStruct.RTC_Minutes / 10 + '0'; //分
    time_buffer[4] = RTC_TimeStruct.RTC_Minutes % 10 + '0';
    
    time_buffer[6] = RTC_TimeStruct.RTC_Seconds / 10 + '0'; //秒
    time_buffer[7] = RTC_TimeStruct.RTC_Seconds % 10 + '0';

    for(u8 i=0;i<128;i++)
    {
        user_ch[i] = character[(time_buffer[i / 16] - ' ') * 16 + (i % 16)];
    }
    
    //printf("show the time\r\n");
    cur_mtrid=NOM;
    CH_display();
    cur_mtrid = pre_mtrid;
    memset(user_ch,0,sizeof(user_ch));

}


void Function(void)
{

    if (rx_flag == 0) //已完成数据包的转移
    {

        if (cur_switch == 0) //开关被关闭
        {
            if (pre_switch == 1) //屏幕未熄灭
            {
                for (u8 i = 0; i < TIME; i++)
                {
                    RGB_BLACK(Pixel_S1_NUM); //熄灭屏幕
                }
                pre_switch = 0;
            }
        }
        else //开关被打开
        {
            pre_switch = 1;
            cur_switch = 1;
            if (ch_switch == 1)
            {
                cur_mtrid = NOM+1;
                CH_display();
                cur_mtrid = pre_mtrid;
            }
            else if(tim_switch==1)
            {
                TIM_display();
            }
            else 
            {
                MTR_display();
            }


            //printf("mission complete\r\n");
        }
        

        
    }
    if(ch_flag==1)
    {
         for (u16 i = 0; i < ch_buffer[20] * 16; i++)
            {
                user_ch[i] = character[(ch_buffer[i / 16] - ' ') * 16 + (i % 16)];
            }
            ch_flag=0;
            printf("装载完成:%d\r\n",ch_buffer[20]);
    }
    
}

