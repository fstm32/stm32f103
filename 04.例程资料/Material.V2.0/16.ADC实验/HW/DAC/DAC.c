/*********************************************************************************************************
* ģ�����ƣ�DAC.c
* ժ    Ҫ��DACģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2020��01��01�� 
* ��    �ݣ�
* ע    �⣺                                                                  
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DAC.h"
#include "Wave.h"
#include "stm32f10x_conf.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define DAC_DHR12R1_ADDR    ((u32)0x40007408)   //DAC1�ĵ�ַ��12λ�Ҷ��룩

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructDACWave s_strDAC1WaveBuf;  //�洢DAC1�������ԣ��������ε�ַ�͵���

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void ConfigTimer4(u16 arr, u16 psc);            //����TIM4
static  void ConfigDAC1(void);                          //����DAC1
static  void ConfigDMA2Ch3ForDAC1(StructDACWave wave);  //����DMA2ͨ��3
                                          
/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigTimer4
* �������ܣ�����TIM4
* ���������arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺DACÿ������֮���ʱ���Ϊ(arr+1)*(psc+1)/72(us)��100���㣬���ڼ�Ϊ1ms
*********************************************************************************************************/
static  void ConfigTimer4(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;   //TIM_TimeBaseStructure���ڴ�Ŷ�ʱ���Ĳ���
  
  //ʹ��RCC���ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //ʹ�ܶ�ʱ����ʱ��
  
  //����TIM4
  TIM_DeInit(TIM4);                                 //����Ϊȱʡֵ
  TIM_TimeBaseStructure.TIM_Period        = arr;    //�����Զ���װ��ֵ   
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;    //����Ԥ��Ƶ��ֵ    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //����ʱ�ӷָtDTS = tCK_INT
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //�������ϼ���ģʽ 
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                //���ݲ�����ʼ����ʱ��

  TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);          //ѡ������¼�Ϊ��������

  TIM_Cmd(TIM4, ENABLE);  //ʹ�ܶ�ʱ��
} 

/*********************************************************************************************************
* �������ƣ�ConfigDAC1
* �������ܣ�����DAC1��DAC1ͨ��PA4���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺Ϊ����������źͶ��⹦�ģ�GPIOģʽ��������Ϊģ������
*********************************************************************************************************/
static  void ConfigDAC1(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; //GPIO_InitStructure���ڴ��GPIO�Ĳ���
  DAC_InitTypeDef   DAC_InitStructure;  //DAC_InitStructure���ڴ��DAC�Ĳ���

  //ʹ��RCC���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOA��ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);    //ʹ��DAC��ʱ��
  
  //����DAC1��GPIO
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;            //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //����I/O����ٶ�
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;         //������������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                 //���ݲ�����ʼ��GPIO
  
  //����DAC1
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T4_TRGO;            //����DAC����
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; //�رղ��η�����
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //������LSFRλ0/���ǲ���ֵ����1
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;   //ʹ��DAC�������
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);    //��ʼ��DACͨ��1

  DAC_DMACmd(DAC_Channel_1, ENABLE);              //ʹ��DACͨ��1��DMAģʽ
 
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);        //����Ϊ12λ�Ҷ������ݸ�ʽ
  
  DAC_Cmd(DAC_Channel_1, ENABLE);                 //ʹ��DACͨ��1
}

/*********************************************************************************************************
* �������ƣ�ConfigDMA2Ch3ForDAC1
* �������ܣ�����DMA2ͨ��3
* ���������wave���������ε�ַ�͵���
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void ConfigDMA2Ch3ForDAC1(StructDACWave wave)
{  
  DMA_InitTypeDef   DMA_InitStructure;  //DMA_InitStructure���ڴ��DMA�Ĳ���
  NVIC_InitTypeDef  NVIC_InitStructure; //NVIC_InitStructure���ڴ��NVIC�Ĳ���

  //ʹ��RCC���ʱ��
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  //ʹ��DMA2��ʱ��  
  
  //����DMA2_Channel3
  DMA_DeInit(DMA2_Channel3);  //��DMA1_CH1�Ĵ�������ΪĬ��ֵ
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDR;     //���������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr     = wave.waveBufAddr;     //���ô洢����ַ
  DMA_InitStructure.DMA_BufferSize         = wave.waveBufSize;     //����Ҫ�������������Ŀ                                              
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;//����Ϊ�洢��������ģʽ
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;  //��������Ϊ�ǵ���ģʽ
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;       //���ô洢��Ϊ����ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //�����������ݳ���Ϊ����
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;	    //���ô洢�����ݳ���Ϊ����
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;    //����Ϊѭ��ģʽ
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;    //����Ϊ�����ȼ�
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;      //��ֹ�洢�����洢������
  DMA_Init(DMA2_Channel3, &DMA_InitStructure); //���ݲ�����ʼ��DMA2_Channel3

  //����NVIC
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;   //�ж�ͨ����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                            //���ݲ�����ʼ��NVIC 

  DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);  //ʹ��DMA2_Channel3�Ĵ�������ж� 

  DMA_Cmd(DMA2_Channel3, ENABLE);                  //ʹ��DMA2_Channel3
}

/*********************************************************************************************************
* �������ƣ�DMA2_Channel3_IRQHandler
* �������ܣ�DMA2_Channel3���жϷ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_IT_TC3))  //�ж�DMA2_Channel3��������ж��Ƿ���
  {
    NVIC_ClearPendingIRQ(DMA2_Channel3_IRQn);  //���DMA2_Channel3�жϹ���
    DMA_ClearITPendingBit(DMA2_IT_GL3);        //���DMA2_Channel3��������жϱ�־

    ConfigDMA2Ch3ForDAC1(s_strDAC1WaveBuf);    //����DMA2ͨ��3
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitDAC
* �������ܣ���ʼ��DACģ�� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void InitDAC(void)
{              
  s_strDAC1WaveBuf.waveBufAddr  = (u32)GetSineWave100PointAddr(); //���ε�ַ
  s_strDAC1WaveBuf.waveBufSize  = 100;                            //���ε���  

  ConfigDAC1(); //����DAC1
  ConfigTimer4(799, 719);   //100KHz��������800Ϊ8ms 
  ConfigDMA2Ch3ForDAC1(s_strDAC1WaveBuf); //����DMA2ͨ��3
}

/*********************************************************************************************************
* �������ƣ�SetDACWave
* �������ܣ�����DAC�������� 
* ���������wave���������ε�ַ�͵���
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void SetDACWave(StructDACWave wave)
{
  s_strDAC1WaveBuf = wave;  //����wave����DAC��������	
}
