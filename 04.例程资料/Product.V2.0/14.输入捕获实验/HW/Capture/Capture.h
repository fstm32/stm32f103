/*********************************************************************************************************
* 模块名称: Capture.h
* 摘    要: Capture模块
* 当前版本: 1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日  
* 内    容:
* 注    意: none                                                                  
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/
#ifndef _CAPTURE_H_
#define _CAPTURE_H_

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

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitCapture(void);              //初始化Capture模块
u8    GetCaptureVal(i32* pCapVal);    //获取捕获时间，返回值为1表示捕获成功，此时*pCapVal才有意义
 
#endif
