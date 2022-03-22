/*********************************************************************************************************
* 模块名称：RunClock.c
* 摘    要：RunClock模块
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日 
* 内    容：
* 注    意：none                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "RunClock.h"
#include <stdio.h>

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static  i16 s_iHour;
static  i16 s_iMin ;
static  i16 s_iSec ;

static  u8  s_iClockPauseFlag = 0;  //TRUE-暂停，FALSE-正常运行
 
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitRunClock
* 函数功能：初始化RunClock模块 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void  InitRunClock(void)
{
  s_iHour = 0;
  s_iMin  = 0;
  s_iSec  = 0;
}

/*********************************************************************************************************
* 函数名称：RunClockPer2Ms
* 函数功能：计数模块，每2ms调用一次 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void  RunClockPer2Ms(void)
{
  static i16 s_iCnt500 = 0;

  if(499 <= s_iCnt500 && 0 == s_iClockPauseFlag)
  {
    if(59 <= s_iSec)
    {
      if(59 <= s_iMin)
      {
        if(23 <= s_iHour)
        {
          s_iHour = 0;
        }
        else
        {
          s_iHour++;
        }
        s_iMin = 0;
      }
      else
      {
        s_iMin++;
      }
      s_iSec = 0;
    }
    else
    {
      s_iSec++;
    }
    s_iCnt500 = 0;
  }
  else
  {
    s_iCnt500++;
  }
}

/*********************************************************************************************************
* 函数名称：PauseClock
* 函数功能：实现时钟的启动和暂停 
* 输入参数：flag，1-暂停，0-启动
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/      
void  PauseClock(u8 flag)
{
  s_iClockPauseFlag = flag;  
}

/*********************************************************************************************************
* 函数名称：GetTimeVal
* 函数功能：获取当前的时间值 
* 输入参数：时间值的类型
* 输出参数：void
* 返 回 值：获取的时间值，小时、分钟或秒
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
i16   GetTimeVal(u8 type)
{
  i16 timeVal;

  switch(type)
  {
  case TIME_VAL_HOUR:
    timeVal = s_iHour;
    break;
  case TIME_VAL_MIN:
    timeVal = s_iMin;
    break;
  case TIME_VAL_SEC:
    timeVal = s_iSec;
    break;
  default:
    break;
  } 
  
  return(timeVal);
}

/*********************************************************************************************************
* 函数名称：SetTimeVal
* 函数功能：设置当前的时间值 
* 输入参数：type-时间值的类型，timeVal-要设置的时间值
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/      
void  SetTimeVal(u8 type, i16 timeVal)
{
  switch(type)
  {
    case TIME_VAL_HOUR:
      s_iHour = timeVal;
      break;
    case TIME_VAL_MIN:
      s_iMin  = timeVal;
      break;
    case TIME_VAL_SEC:
      s_iSec  = timeVal;
      break;
    default:
      break;
  } 
}   

/*********************************************************************************************************
* 函数名称：DispTime
* 函数功能：显示当前的时间
* 输入参数：hour，min，sec
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void  DispTime(i16 hour, i16 min, i16 sec)    //显示当前的时间
{
  printf("BTC's time: %02d:%02d:%02d\n", hour, min, sec);
}
