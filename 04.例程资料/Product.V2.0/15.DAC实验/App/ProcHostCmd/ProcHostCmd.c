/*********************************************************************************************************
* 模块名称：ProcHostCmd.c
* 摘    要：ProcHostCmd模块
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

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "ProcHostCmd.h"
#include "PackUnpack.h"
#include "SendDataToHost.h"
#include "DAC.h"
#include "Wave.h"

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
static u8  OnGenWave(u8* pMsg);  //生成波形的响应函数

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：OnGenWave
* 函数功能：生成波形的响应函数
* 输入参数：pMsg
* 输出参数：void
* 返 回 值：应答消息
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static u8 OnGenWave(u8* pMsg) 
{  
  StructDACWave wave;   //DAC波形属性
  
  if(pMsg[0] == 0x00)                      
  {
    wave.waveBufAddr  = (u32)GetSineWave100PointAddr();  //获取正弦波数组的地址 
  }
  else if(pMsg[0] == 0x01)
  {
    wave.waveBufAddr  = (u32)GetTriWave100PointAddr();   //获取三角波数组的地址 
  }
  else if(pMsg[0] == 0x02)
  {
    wave.waveBufAddr  = (u32)GetRectWave100PointAddr();  //获取方波数组的地址 
  }
  
  wave.waveBufSize  = 100;  //波形一个周期点数为100

  SetDACWave(wave);         //设置DAC波形属性
  
  return(CMD_ACK_OK);       //返回命令成功
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitProcHostCmd
* 函数功能：初始化ProcHostCmd模块 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void  InitProcHostCmd(void)
{
  
}

/*********************************************************************************************************
* 函数名称：ProcHostCmd
* 函数功能：处理主机发送来的命令 
* 输入参数：recData
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void ProcHostCmd(u8 recData)
{ 
  u8 ack;                 //存储应答消息
  StructPackType pack;    //包结构体变量
  
  while(UnPackData(recData))   //解包成功
  {
    pack = GetUnPackRslt();    //获取解包结果    
    
    switch(pack.packModuleId)  //模块ID
    {
      case MODULE_WAVE:        //波形信息
        ack = OnGenWave(pack.arrData);                //生成波形
        SendAckPack(MODULE_WAVE, CMD_GEN_WAVE, ack);  //发送命令应答消息包
        break;
      default:     
        break;
    }
  }   
}
