/*********************************************************************************************************
* ģ�����ƣ�ADC.c
* ժ    Ҫ��ADCģ��
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
#include "ADC.h"
#include "stm32f10x_conf.h"
#include "U16Queue.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static u16 s_arrADC1Data;   //���ADCת���������
static StructU16CirQue  s_structADCCirQue;            //ADCѭ������
static u16              s_arrADCBuf[ADC1_BUF_SIZE];   //ADCѭ�����еĻ�����

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ConfigADC1(void);     //����ADC1
static void ConfigDMA1Ch1(void);  //����DMAͨ��1
static void ConfigTimer3(u16 arr, u16 psc); //����TIM3

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigADC1
* �������ܣ�����ADC1
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺ADC123_IN1-PA1
**********************************************************************************************************/
static void ConfigADC1(void)
{                          
  GPIO_InitTypeDef  GPIO_InitStructure; //GPIO_InitStructure���ڴ��GPIO�Ĳ���
  ADC_InitTypeDef   ADC_InitStructure;  //ADC_InitStructure���ڴ��ADC�Ĳ���

  //ʹ��RCC���ʱ��
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); //����ADCʱ�ӷ�Ƶ��ADCCLK=PCLK2/6=12MHz
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1  , ENABLE);  //ʹ��ADC1��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIOA��ʱ��
 
  //����ADC1��GPIO
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;    //��������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN; //������������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //���ݲ�����ʼ��GPIO

  //����ADC1
  ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;  //����Ϊ����ģʽ
  ADC_InitStructure.ADC_ScanConvMode       = ENABLE;                //ʹ��ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;               //��ֹ����ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T3_TRGO;  //ʹ��TIM3����
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;   //����Ϊ�Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel       = 1; //����ADC��ͨ����Ŀ
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //���ò���ʱ��Ϊ239.5������

  ADC_DMACmd(ADC1, ENABLE);                   //ʹ��ADC1��DMA
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);      //ʹ���ⲿ�¼�����ADCת��
  ADC_Cmd(ADC1, ENABLE);                      //ʹ��ADC1
  ADC_ResetCalibration(ADC1);                 //����ADC��λУ׼������RSTCAL��ֵΪ1
  while(ADC_GetResetCalibrationStatus(ADC1)); //��ȡ���ж�RSTCAL��RSTCALΪ0����while���
  ADC_StartCalibration(ADC1);                 //����ADCУ׼������CAL��ֵΪ1
  while(ADC_GetCalibrationStatus(ADC1));      //��ȡ���ж�CAL��CALΪ0����while���
}

/*********************************************************************************************************
* �������ƣ�ConfigDMA1Ch1
* �������ܣ�����DMAͨ��1
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
**********************************************************************************************************/
static void ConfigDMA1Ch1(void)
{
  DMA_InitTypeDef DMA_InitStructure;  //DMA_InitStructure���ڴ��DMA�Ĳ���
  
  //ʹ��RCC���ʱ��
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //ʹ��DMA1��ʱ��
  
  //����DMA1_Channel1
  DMA_DeInit(DMA1_Channel1);  //��DMA1_CH1�Ĵ�������ΪĬ��ֵ
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);           //���������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)&s_arrADC1Data;        //���ô洢����ַ
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;           //����Ϊ���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize         = 1;                               //����Ҫ�������������Ŀ
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       //��������Ϊ�ǵ���ģʽ
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;            //���ô洢��Ϊ����ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //�����������ݳ���Ϊ����
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;     //���ô洢�����ݳ���Ϊ����
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;               //����Ϊѭ��ģʽ
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;             //����Ϊ�е����ȼ�
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                 //��ֹ�洢�����洢������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //���ݲ�����ʼ��DMA1_Channel1
  
  DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMA1_Channel1
}

/*********************************************************************************************************
* �������ƣ�ConfigTimer3
* �������ܣ����ö�ʱ��TIM3�Ĳ���
* ���������arr-�Զ���װֵ��psc-ʱ��Ԥ��Ƶ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺ 
**********************************************************************************************************/
static void ConfigTimer3(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //TIM_TimeBaseStructure���ڴ��TIM3�Ĳ���
  NVIC_InitTypeDef NVIC_InitStructure;            //NVIC_InitStructure���ڴ��NVIC�Ĳ���
    
  //ʹ��RCC���ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //ʹ��TIM3��ʱ��

  //����TIM3
  TIM_TimeBaseStructure.TIM_Period        = arr;  //�����Զ���װ��ֵ
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;  //����Ԥ��Ƶ��ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //����ʱ�ӷָtDTS = tCK_INT
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //�������ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //���ݲ�����ʼ����ʱ��  
  
  TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);          //ѡ������¼�Ϊ��������
  
  TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);                     //ʹ�ܶ�ʱ���ĸ����ж�
  
  //����NVIC
  NVIC_InitStructure.NVIC_IRQChannel      = TIM3_IRQn;          //�ж�ͨ����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     //������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;     //���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd   = ENABLE;             //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                               //���ݲ�����ʼ��NVIC
  
  TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��
}
 
/*********************************************************************************************************
* �������ƣ�TIM3_IRQHandler
* �������ܣ�TIM3�жϷ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
**********************************************************************************************************/
void TIM3_IRQHandler(void)
{  
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //�ж϶�ʱ�������ж��Ƿ���
  {
    TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);     //�����ʱ�������жϱ�־ 
  }  

  WriteADCBuf(s_arrADC1Data);   //��ADC������д������
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitADC
* �������ܣ���ʼ��ADCģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
**********************************************************************************************************/
void InitADC(void)
{
  ConfigTimer3(799, 719);   //100KHz��������800Ϊ8ms
  ConfigADC1();             //����ADC1
  ConfigDMA1Ch1();          //����DMA1��ͨ��1

  InitU16Queue(&s_structADCCirQue, s_arrADCBuf, ADC1_BUF_SIZE); //��ʼ��ADC������  
}

/*********************************************************************************************************
* �������ƣ�WriteADCBuf
* �������ܣ���ADC������д������
* ���������d-��д�������
* ���������void
* �� �� ֵ���ɹ���־λ��1Ϊ�ɹ���0Ϊ���ɹ�
* �������ڣ�2018��01��01��
* ע    �⣺
**********************************************************************************************************/
u8 WriteADCBuf(u16 d)
{
  u8 ok = 0;  //����ȡ�ɹ���־λ��ֵ����Ϊ0

  ok = EnU16Queue(&s_structADCCirQue, &d, 1); //���

  return ok;  //���ض�ȡ�ɹ���־λ��ֵ
}

/*********************************************************************************************************
* �������ƣ�ReadADCBuf
* �������ܣ���ADC��������ȡ����
* ���������p-��ȡ�����ݴ�ŵ��׵�ַ
* ���������void
* �� �� ֵ���ɹ���־λ��1Ϊ�ɹ���0Ϊ���ɹ�
* �������ڣ�2018��01��01��
* ע    �⣺
**********************************************************************************************************/
u8 ReadADCBuf(u16* p)
{
  u8 ok = 0;  //����ȡ�ɹ���־λ��ֵ����Ϊ0

  ok = DeU16Queue(&s_structADCCirQue, p, 1); //����

  return ok;  //���ض�ȡ�ɹ���־λ��ֵ
}
