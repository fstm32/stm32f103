/*********************************************************************************************************
* 模块名称: RCC.c
* 摘    要: 
* 当前版本: 1.0.0
* 作    者: SZLY(COPYRIGHT 2018 SZLY. All rights reserved.)
* 完成日期: 2018年01月01日
* 内    容:
* 注    意: none                                                                  
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "RCC.h"
#include "stm32f10x_conf.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  ConfigRCC(void);    //配置RCC

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称: ConfigRCC
* 函数功能: 配置RCC 
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意: 配置的时钟如下所述：
*           （0）外部晶振HSE=8MHz
*           （1）PLLCLK（PLL时钟）=HSE*9=72MHz
*           （2）SYSCLK（系统时钟）=PLLCLK=72 MHz
*           （3）HCLK（AHB总线时钟）=SYSCLK=72MHz
*           （4）PCLK1（APB1总线时钟）=HCLK/2=36MHz
*           （5）PCLK2（APB2总线时钟）=HCLK=72MHz
*           （6）ADCCLK（ADC时钟）=PCLK2/4=72/4=18MHz 
*********************************************************************************************************/
static void ConfigRCC(void)
{
  ErrorStatus HSEStartUpStatus;               //定义枚举变量HSEStartUpStatus，用来标志外部高速晶振的状态
  
  RCC_DeInit();                               //将外设RCC寄存器重设为默认值

  RCC_HSEConfig(RCC_HSE_ON);                  //使能外部高速晶振

  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //等待外部高速晶振稳定
  
  if(HSEStartUpStatus == SUCCESS)             //外部高速晶振成功稳定   
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //使能flash预读取缓冲区

    FLASH_SetLatency(FLASH_Latency_2);        //设置代码延时值，FLASH_Latency_2，2延时周期
  
    RCC_HCLKConfig(RCC_SYSCLK_Div1);          //设置高速AHB时钟（HCLK），RCC_SYSCLK_Div1，HCLK = SYSCLK 
  
    RCC_PCLK2Config(RCC_HCLK_Div1);           //设置高速APB2时钟（PCLK2），RCC_HCLK_Div1，PCLK2 = HCLK

    RCC_PCLK1Config(RCC_HCLK_Div2);           //设置低速APB1时钟（PCLK1），RCC_HCLK_Div2，PCLK1 = HCLK/2

    //RCC_ADCCLKConfig(RCC_PCLK2_Div4);       //设置ADC时钟（ADCCLK），RCC_PCLK2_Div4，ADCCLK = PCLK2/4 
  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//设置PLL时钟源及倍频系数，PLLCLK = 8MHz*9 = 72MHz

    RCC_PLLCmd(ENABLE);                       //使能PLL

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)  //等待锁相环输出稳定
    {
      
    }

    //设置HSI/HSE/PLL为系统时钟
    //RCC_SYSCLKSource_HSI
    //RCC_SYSCLKSource_HSE
    //RCC_SYSCLKSource_PLLCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);         //将锁相环输出设置为系统时钟

    //等待HSI/HSE/PLL成功用于系统时钟的时钟源
    //0x00-HSI作为系统时钟
    //0x04-HSE作为系统时钟
    //0x08-PLL作为系统时钟
    while(RCC_GetSYSCLKSource() != 0x08)               //等待PLL成功用于系统时钟的时钟源
    {
      
    }
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitRCC
* 函数功能: 初始化 RCC
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意: 
*********************************************************************************************************/
void InitRCC(void)
{
  ConfigRCC();          //配置RCC
}
