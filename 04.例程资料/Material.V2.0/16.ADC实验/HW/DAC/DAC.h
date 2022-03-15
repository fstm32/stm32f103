/*********************************************************************************************************
* 模块名称：DAC.h
* 摘    要：DAC模块
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
#ifndef _DAC_H_
#define _DAC_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef struct
{
  u32 waveBufAddr;  //波形地址
  u32 waveBufSize;  //波形点数
}StructDACWave;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitDAC(void);  //初始化DAC模块           
void  SetDACWave(StructDACWave wave); //设置DAC波形属性，包括波形地址和点数

#endif
