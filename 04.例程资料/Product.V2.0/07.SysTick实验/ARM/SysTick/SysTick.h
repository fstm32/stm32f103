/*********************************************************************************************************
* 模块名称：SysTick.h
* 摘    要：SysTick模块，包含SysTick模块初始化以及微秒和毫秒级延时函数
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日 
* 内    容：
* 注    意：                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容： 
* 修改文件：
*********************************************************************************************************/
#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"
#include "stm32f10x.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitSysTick(void);      //初始化SysTick模块
void  DelayNus(__IO u32 nus); //微秒级延时函数
void  DelayNms(__IO u32 nms); //毫秒级延时函数
 
#endif
