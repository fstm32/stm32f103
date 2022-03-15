/*********************************************************************************************************
* ģ�����ƣ�Capture.c
* ժ    Ҫ��Captureģ��
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
#include "Capture.h"
#include "stm32f10x_conf.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//s_iCaptureSts�е�bit7Ϊ������ɵı�־��bit6Ϊ�����½��ر�־��bit5-bit0Ϊ�����½��غ�ʱ������Ĵ���
static  u8  s_iCaptureSts = 0;     //����״̬ 
static  u16 s_iCaptureVal;         //����ֵ
 
/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void ConfigTIM5ForCapture(u16 arr, u16 psc);    //����TIM3

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigTIM5ForCapture
* �������ܣ�����TIM3 
* ���������arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01�� 
* ע    �⣺�˴��ݶ�ʹ�ö�ʱ��TIM5��CH1��PA0���������벶�񣬲���PA0��KEY3���ϵ͵�ƽ������
*********************************************************************************************************/
static  void ConfigTIM5ForCapture(u16 arr, u16 psc)
{ 
  GPIO_InitTypeDef        GPIO_InitStructure;    //GPIO_InitStructure���ڴ��GPIO�Ĳ���
  TIM_TimeBaseInitTypeDef TIMx_TimeBaseStructure;//TIM_TimeBaseStructure���ڴ�Ŷ�ʱ���Ļ�������
  TIM_ICInitTypeDef       TIMx_ICInitStructure;  //TIMx_ICInitStructure���ڴ�Ŷ�ʱ����ͨ������
  NVIC_InitTypeDef        NVIC_InitStructure;    //NVIC_InitStructure���ڴ��NVIC�Ĳ���

  //ʹ��RCC���ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //ʹ��TIM5��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ�ܲ����GPIOA��ʱ��
  
  //����PA0����ӦTIM5��CH1
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;            //��������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //��������ģʽ 
  GPIO_Init(GPIOA, &GPIO_InitStructure);                //���ݲ�����ʼ��GPIO
  GPIO_SetBits(GPIOA, GPIO_Pin_0);  //�������Ӧ��������Ϊ�ߵ�ƽ
  
  //����TIM5
  TIMx_TimeBaseStructure.TIM_Period        = arr;                     //�趨�������Զ���װֵ 
  TIMx_TimeBaseStructure.TIM_Prescaler     = psc;                     //����TIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  
  TIMx_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //����ʱ�ӷָ�
  TIMx_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;      //���ö�ʱ��TIMxΪ���ϼ���ģʽ
  TIM_TimeBaseInit(TIM5, &TIMx_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
  //����TIM5��CH1Ϊ���벶��
  //CC1S = 01��CC1ͨ��������Ϊ���룬����ͨ��IC1ӳ�䵽��ʱ������TI1��
  TIMx_ICInitStructure.TIM_Channel        = TIM_Channel_1;            //��������ͨ��ͨ�� 
  TIMx_ICInitStructure.TIM_ICPolarity     = TIM_ICPolarity_Falling;   //����Ϊ�½��ز���
  TIMx_ICInitStructure.TIM_ICSelection    = TIM_ICSelection_DirectTI; //����Ϊֱ��ӳ�䵽TI1
  TIMx_ICInitStructure.TIM_ICPrescaler    = TIM_ICPSC_DIV1;           //����Ϊÿһ�����ض����񣬲�׽����Ƶ 
  TIMx_ICInitStructure.TIM_ICFilter       = 0x08;                     //���������˲���
  TIM_ICInit(TIM5, &TIMx_ICInitStructure);         //���ݲ�����ʼ��TIM5��CH1
  
  //����NVIC
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM5_IRQn;     //�ж�ͨ����   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;             //������ռ���ȼ�   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;             //���������ȼ�   
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;        //ʹ���ж�   
  NVIC_Init(&NVIC_InitStructure);                                       //���ݲ�����ʼ��NVIC    
  
  TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE); //ʹ�ܶ�ʱ���ĸ����жϺ�CC1IE�����ж� 
  
  TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
}

/*********************************************************************************************************
* �������ƣ�TIM5_IRQHandler
* �������ܣ�TIM5�жϷ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01�� 
* ע    �⣺
*********************************************************************************************************/
void TIM5_IRQHandler(void)
{ 
  if((s_iCaptureSts & 0x80) == 0) //���λΪ0����ʾ����δ���
  {  
    //�ߵ�ƽ����ʱ��TIMx����������¼�
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)        
    {    
      if(s_iCaptureSts & 0x40)    //�������������ǰһ���Ѿ����񵽵͵�ƽ
      {
        //TIM_APR 16λԤװ��ֵ����CNT > 65536-1��2^16 - 1��ʱ�����
        //��������(s_iCaptureSts & 0x3F)++����0x40 �������������0
        if((s_iCaptureSts & 0x3F) == 0x3F)  //�ﵽ���������͵�ƽ̫��
        {
          s_iCaptureSts |= 0x80;  //ǿ�Ʊ�ǳɹ�������һ��
          s_iCaptureVal = 0xFFFF; //����ֵΪ0xFFFF
        } 
        else
        {
          s_iCaptureSts++;        //��Ǽ��������һ��
        }
      }
    }
    
    if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) //���������¼�
    { 
      if(s_iCaptureSts & 0x40)  //bit6Ϊ1�����ϴβ����½��أ���ô��β���������
      {
        s_iCaptureSts |= 0x80;  //��ɲ��񣬱�ǳɹ�����һ��������
        s_iCaptureVal = TIM_GetCapture1(TIM5);  //s_iCaptureVa��¼����ȽϼĴ�����ֵ
        //CC1P=1 ����Ϊ�½��ز���Ϊ�´β�����׼��
        TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);   
      }
      else  //bit6Ϊ0����ʾ�ϴ�û�����½��أ����ǵ�һ�β����½���
      {
        s_iCaptureSts = 0;  //����������
        s_iCaptureVal = 0;  //����ֵΪ0
                                                             
        TIM_SetCounter(TIM5, 0);  //���üĴ�����ֵΪ0
                                                             
        s_iCaptureSts |= 0x40;    //bit6��Ϊ1����ǲ������½���
        
        TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);  //CC1P=0 ����Ϊ�����ز���
      }    
    } 
  }
 
  TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update);    //����жϱ�־λ
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitCapture
* �������ܣ���ʼ��Captureģ�� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01�� 
* ע    �⣺
*********************************************************************************************************/
void  InitCapture(void)
{
  //�������ﵽ���װ��ֵ0xFFFF��������������72MHz/��72-1+1��=1MHz��Ƶ�ʼ���
  ConfigTIM5ForCapture(0xFFFF, 72 - 1);  
}

/*********************************************************************************************************
* �������ƣ�GetCaptureVal
* �������ܣ���ȡ����ʱ�䣬����ֵΪ1��ʾ����ɹ�����ʱ*pCapVal�������� 
* ���������void
* ���������pCalVal�����񵽵�ֵ�ĵ�ַ
* �� �� ֵ��ok��1-��ȡ�ɹ�
* �������ڣ�2018��01��01�� 
* ע    �⣺
*********************************************************************************************************/
u8   GetCaptureVal(i32* pCapVal)
{
  u8  ok = 0;
  
  if(s_iCaptureSts & 0x80)              //���λΪ1����ʾ�ɹ������������أ���ȡ�����������־��
  {
    ok = 1;                             //����ɹ�
    (*pCapVal)  = s_iCaptureSts & 0x3F; //ȡ����6λ��������ֵ����(*pCapVal)���õ��������
    (*pCapVal) *= 65536;//��������������Ϊ2^16=65536����������������õ����ʱ���ܺͣ���1/1MHz=1usΪ��λ��
    (*pCapVal) += s_iCaptureVal;        //�������һ�αȽϲ���Ĵ�����ֵ���õ��ܵĵ͵�ƽʱ��

    s_iCaptureSts = 0;                  //����Ϊ0��������һ�β���
  }

  return(ok);                           //�����Ƿ񲶻�ɹ��ı�־
}
