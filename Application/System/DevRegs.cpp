//-----------------------------------------------------------------------------
/*
 File        : DevRegs.c
 Version     : V1.01
 By          : 卫荣平

 Description :分配系统寄存器  实现访问方法

 Date       : 2017.8.19
*/
//-----------------------------------------------------------------------------
#include "DevRegs.h"

#include "DevIntf.h"
#include "DevBuffer.h"
#include "DevFixed.h"
#include "DevTypes.h"
//#include "DevDynaPswd.h"

#ifndef __vmSIMULATOR__
  #include "rtc.h"
  #include "dio.h"
//  #include "Relay.h"
  #include "usart.h"
  #include "cmsis_os.h"
#endif

#include "DevFunc.h"

#include <string.h>
#include <Dev_Cfg.h>
#include <cstdint>
//=============================================================================
// 全局数据
//-----------------------------------------------------------------------------
#define SUPER_PASSWORD             3300
#define SUPER_PASSWORD2            9220
//=============================================================================
// 全局方法引用
//-----------------------------------------------------------------------------

//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------

//=============================================================================
// 局部数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// 本地数据
//-----------------------------------------------------------------------------
// 用于的装置配置
extern TDeviceFixed DevCfgForEdit;
//  #define GetEditNIC1Cfg        (&DevCfgForEdit.NICConfig[0])
//  #define GetEditNIC2Cfg        (&DevCfgForEdit.NICConfig[1])
//  #define GetEditNIC1SktCfg(n)  (&DevCfgForEdit.NIC1SocketsConfig[n + 1])
//  #define GetEditNIC2SktCfg(n)  (&DevCfgForEdit.NIC2SocketsConfig[n + 1])

//=============================================================================
// 本地方法
//-----------------------------------------------------------------------------
// 读取功能寄存器
static uint32_t _ReadFuncRegs(uint32_t uRegNum)
{

  uint32_t uRes = 0;

  switch( uRegNum )
    {
    // Device Options
    case REG_FN_PROTECT_EN:
    case REG_FN_SOE_AUTORET:
    case REG_FN_CT_1A:
    case REG_FN_VOLTAGE_MODE:
      {
      uint32_t uMask = (1 << (uRegNum - REG_FUNCTION));
      if( 0 != (DevCfgForEdit.Options & uMask) )
        uRes = STATE_TRUE;
      else
        uRes = STATE_FALSE;
      break;
      }

    // 设备参数
    case REG_DEVFUNC:
      {
      uRes = DevConfig.DevFunc;
      break;
      }

    case REG_FN_DEVFUNC:
      {
      uRes = DevCfgForEdit.DevFunc;
      break;
      }

    case REG_ACTV_HOLD_BLK:
      {
      uRes = DevConfig.ActiveHoldingBlock;
      break;
      }

    case REG_FN_ACTV_HOLD_BLK:
      {
      uRes = DevCfgForEdit.ActiveHoldingBlock;
      break;
      }

    case REG_EDIT_HOLD_BLK:
    case REG_FN_EDIT_HOLD_BLK:
    case REG_FN_COPY_HOLD_BLK:
      {
      uRes = DevCache.DeviceState[REG_EditHoldBlock - REG_DEVSTATE];
      break;
      }

//    case REG_DISTRIBUTOR:
//      {
//      uRes = DevConfig.Distributor;
//      break;
//      }

//    case REG_FN_Distributor:
//      {
//      uRes = DevCfgForEdit.Distributor;
//      break;
//      }

    case REG_LANGUAGE:
      {
      uRes = DevConfig.Language;
      break;
      }

    case REG_FN_Language:
      {
      uRes = DevCfgForEdit.Language;
      break;
      }

     case REG_PASSWORD:
      {
      uRes = DevConfig.Password;
      break;
      }

    case REG_FN_PASSWORD:
      {
      uRes = DevCfgForEdit.Password;
      break;
      }

     case REG_PASSWORD2:
      {
      uRes = DevConfig.Password2;
      break;
      }

    case REG_FN_PASSWORD2:
      {
      uRes = DevCfgForEdit.Password2;
      break;
      }

//    case REG_PASSWORDx:
//      {
//      // 获取动态口令码
//      uRes = DynaPswd_GetApplyCode();

//      break;
//      }

//    case REG_FN_PT_RATE:
//    //case REG_FN_PT2_RATE:
//    case REG_FN_CT_RATE:
//    //case REG_FN_CT2_RATE:
//      {
//      uRes = DevCfgForEdit.Configs[(uRegNum - REG_FN_PT_RATE) + REG_PT_RATE - REG_DEVCONFIG];
//      break;
//      }

//    // 遥信延时
//    case REG_FN_DI_JEDELAY:
//      {
//      uRes = DevCfgForEdit.Configs[REG_DI_JE_DELAY - REG_DEVCONFIG];
//      break;
//      }

//    // 遥控超时
//    case REG_FN_RC_OVERTIME:
//      {
//      uRes = DevCfgForEdit.Configs[REG_RC_OVERTIME - REG_DEVCONFIG];
//      break;
//      }

#ifdef REG_LLCurr
    // 电流死区
    case REG_FN_LLCurr:
      {
      uRes = DevCfgForEdit.Configs[REG_LLCurr - REG_DEVCONFIG];
      break;
      }
#endif

#ifdef REG_LLVolt
    // 电压死区
    case REG_FN_LLVolt:
      {
      uRes = DevCfgForEdit.Configs[REG_LLVolt - REG_DEVCONFIG];
      break;
      }
#endif

//    // 直流配置
//    case REG_FN_DC0Min:
//    case REG_FN_DC0Max:
//    case REG_FN_DC1Min:
//    case REG_FN_DC1Max:
//    case REG_FN_DC2Min:
//    case REG_FN_DC2Max:
//    case REG_FN_DC3Type:
//    case REG_FN_DC3Min:
//    case REG_FN_DC3Max:
//    case REG_FN_DC4Type:
//    case REG_FN_DC4Min:
//    case REG_FN_DC4Max:
//    case REG_FN_DC5Type:
//    case REG_FN_DC5Min:
//    case REG_FN_DC5Max:
//      {
//      uRegNum  = (uRegNum - REG_FN_DC0Min) + REG_DC0Min - REG_DEVCONFIG;
//      int iRes = (int)DevCfgForEdit.Configs[uRegNum];
//      uRes = (uint32_t)iRes;
//      if( 0 != (DevCfgForEdit.Configs[uRegNum] & 0x8000) )
//        uRes |= 0xFFFF0000;
//      break;
//      }

#ifndef __vmSIMULATOR__
    // 日期时间
    case REG_FN_DATETIME:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = (uint32_t)(&DevCache.DateTime);
      break;
      }

    case REG_DATE_YEAR:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Year;
      break;
      }

    case REG_DATE_MONTH:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Month;
      break;
      }

    case REG_DATE_DAY:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Day;
      break;
      }

    case REG_TIME_HOUR:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Hours;
      break;
      }

    case REG_TIME_MINUTE:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Minutes;
      break;
      }

    case REG_TIME_SECOND:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.Seconds;
      break;
      }

    case REG_TIME_MSECOND:
      {
      RTC_GetTime( &DevCache.DateTime );
      uRes = DevCache.DateTime.MilSeconds;
      break;
      }
#endif

    //#ifndef __vmSIMULATOR__
//    // 以太网1配置
//    case REG_NIC1_CONFIG:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = (uint32_t)(pNicConfig);
//      break;
//      }

//    case REG_NIC1_ADDR:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = *(uint32_t*)pNicConfig->Addr;
//      break;
//      }
//#endif

//    case REG_NIC1_ADDR1:
//    case REG_NIC1_ADDR2:
//    case REG_NIC1_ADDR3:
//    case REG_NIC1_ADDR4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = pNicConfig->Addr[uRegNum - REG_NIC1_ADDR1];
//      break;
//      }

//    case REG_NIC1_NetMask:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = *(uint32_t*)pNicConfig->Mask;
//      break;
//      }

//    case REG_NIC1_NetMask1:
//    case REG_NIC1_NetMask2:
//    case REG_NIC1_NetMask3:
//    case REG_NIC1_NetMask4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = pNicConfig->Mask[uRegNum - REG_NIC1_NetMask1];
//      break;
//      }

//    case REG_NIC1_NetGate:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = *(uint32_t*)pNicConfig->Gate;
//      break;
//      }

//    case REG_NIC1_NetGate1:
//    case REG_NIC1_NetGate2:
//    case REG_NIC1_NetGate3:
//    case REG_NIC1_NetGate4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = pNicConfig->Gate[uRegNum - REG_NIC1_NetGate1];
//      break;
//      }

//#ifndef __vmSIMULATOR__
//    case REG_NIC1_NetMAC:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = (uint32_t)(pNicConfig->MAC);
//      break;
//      }
//#endif

//    case REG_NIC1_NetMAC1:
//    case REG_NIC1_NetMAC2:
//    case REG_NIC1_NetMAC3:
//    case REG_NIC1_NetMAC4:
//    case REG_NIC1_NetMAC5:
//    case REG_NIC1_NetMAC6:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      uRes = pNicConfig->MAC[uRegNum - REG_NIC1_NetMAC1];
//      break;
//      }

//    // 网络Socket
//    // Socket 1
//    case REG_SKT11_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT11_Addr1:
//    case REG_SKT11_Addr2:
//    case REG_SKT11_Addr3:
//    case REG_SKT11_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT11_Addr1];
//      break;
//      }

//    case REG_SKT11_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT11_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT11_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 2
//    case REG_SKT12_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT12_Addr1:
//    case REG_SKT12_Addr2:
//    case REG_SKT12_Addr3:
//    case REG_SKT12_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT12_Addr1];
//      break;
//      }

//    case REG_SKT12_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT12_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT12_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 3
//    case REG_SKT13_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT13_Addr1:
//    case REG_SKT13_Addr2:
//    case REG_SKT13_Addr3:
//    case REG_SKT13_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT13_Addr1];
//      break;
//      }

//    case REG_SKT13_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT13_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT13_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 4
//    case REG_SKT14_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT14_Addr1:
//    case REG_SKT14_Addr2:
//    case REG_SKT14_Addr3:
//    case REG_SKT14_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT14_Addr1];
//      break;
//      }

//    case REG_SKT14_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT14_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT14_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 5
//    case REG_SKT15_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT15_Addr1:
//    case REG_SKT15_Addr2:
//    case REG_SKT15_Addr3:
//    case REG_SKT15_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT15_Addr1];
//      break;
//      }

//    case REG_SKT15_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT15_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT15_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 6
//    case REG_SKT16_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT16_Addr1:
//    case REG_SKT16_Addr2:
//    case REG_SKT16_Addr3:
//    case REG_SKT16_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT16_Addr1];
//      break;
//      }

//    case REG_SKT16_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT16_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT16_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//#ifndef __vmSIMULATOR__
//    // 以太网2配置
//    case REG_NIC2_CONFIG:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = (uint32_t)(pNicConfig);
//      break;
//      }
//#endif

//    case REG_NIC2_ADDR:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = *(uint32_t*)pNicConfig->Addr;
//      break;
//      }

//    case REG_NIC2_ADDR1:
//    case REG_NIC2_ADDR2:
//    case REG_NIC2_ADDR3:
//    case REG_NIC2_ADDR4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = pNicConfig->Addr[uRegNum - REG_NIC2_ADDR1];
//      break;
//      }

//    case REG_NIC2_NetMask:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = *(uint32_t*)pNicConfig->Mask;
//      break;
//      }

//    case REG_NIC2_NetMask1:
//    case REG_NIC2_NetMask2:
//    case REG_NIC2_NetMask3:
//    case REG_NIC2_NetMask4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = pNicConfig->Mask[uRegNum - REG_NIC2_NetMask1];
//      break;
//      }

//    case REG_NIC2_NetGate:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = *(uint32_t*)pNicConfig->Gate;
//      break;
//      }

//    case REG_NIC2_NetGate1:
//    case REG_NIC2_NetGate2:
//    case REG_NIC2_NetGate3:
//    case REG_NIC2_NetGate4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = pNicConfig->Gate[uRegNum - REG_NIC2_NetGate1];
//      break;
//      }

//#ifndef __vmSIMULATOR__
//    case REG_NIC2_NetMAC:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = (uint32_t)(pNicConfig->MAC);
//      break;
//      }
//#endif

//    case REG_NIC2_NetMAC1:
//    case REG_NIC2_NetMAC2:
//    case REG_NIC2_NetMAC3:
//    case REG_NIC2_NetMAC4:
//    case REG_NIC2_NetMAC5:
//    case REG_NIC2_NetMAC6:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      uRes = pNicConfig->MAC[uRegNum - REG_NIC2_NetMAC1];
//      break;
//      }

//    // 网络Socket
//    // Socket 1
//    case REG_SKT21_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT21_Addr1:
//    case REG_SKT21_Addr2:
//    case REG_SKT21_Addr3:
//    case REG_SKT21_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT21_Addr1];
//      break;
//      }

//    case REG_SKT21_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT21_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT21_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 2
//    case REG_SKT22_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT22_Addr1:
//    case REG_SKT22_Addr2:
//    case REG_SKT22_Addr3:
//    case REG_SKT22_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT22_Addr1];
//      break;
//      }

//    case REG_SKT22_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT22_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT22_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 3
//    case REG_SKT23_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT23_Addr1:
//    case REG_SKT23_Addr2:
//    case REG_SKT23_Addr3:
//    case REG_SKT23_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT23_Addr1];
//      break;
//      }

//    case REG_SKT23_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT23_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT23_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

//    // Socket 4
//    case REG_SKT24_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      uRes = *(uint32_t*)pSocketConfig->Addr;
//      break;
//      }

//    case REG_SKT24_Addr1:
//    case REG_SKT24_Addr2:
//    case REG_SKT24_Addr3:
//    case REG_SKT24_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      uRes = pSocketConfig->Addr[uRegNum - REG_SKT24_Addr1];
//      break;
//      }

//    case REG_SKT24_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      uRes = pSocketConfig->Mode;
//       break;
//      }

//    case REG_SKT24_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      uRes = pSocketConfig->Port;
//      break;
//      }

//    case REG_SKT24_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      uRes = pSocketConfig->Protocol;
//      break;
//      }

#if NUM_UART_PORTS > 0
#ifndef __vmSIMULATOR__
    // 串口1配置
    case REG_UART1_CFG:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      uRes = (uint32_t)pUartConfig;
      break;
      }
#endif

    case REG_UART1_ADDR:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      uRes = pUartConfig->Addr;
      break;
      }

    case REG_UART1_BAUDRATE:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      uRes = pUartConfig->Baudrate;
      break;
      }

    case REG_UART1_PARITY:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      uRes = pUartConfig->Parity;
      break;
      }

    case REG_UART1_PROTOCOL:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      uRes = pUartConfig->Protocol;
      break;
      }
#endif

#if NUM_UART_PORTS > 1
#ifndef __vmSIMULATOR__
    // 串口2配置
    case REG_UART2_CFG:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      uRes = (uint32_t)pUartConfig;
      break;
      }
#endif

    case REG_UART2_ADDR:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      uRes = pUartConfig->Addr;
      break;
      }

    case REG_UART2_BAUDRATE:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      uRes = pUartConfig->Baud;
      break;
      }

    case REG_UART2_PARITY:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      uRes = pUartConfig->Parity;
      break;
      }

    case REG_UART2_PROTOCOL:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      uRes = pUartConfig->Protocol;
      break;
      }
#endif

#ifdef USE_GOOSE
    // GOOSE Publisher
    case REG_GSPUB_APPID:
      {
      uRes = DevCfgForEdit.gsPublisherConfig.uwAppId;
      break;
      }

    case REG_GSPUB_VALNID:
      {
      uRes = DevCfgForEdit.gsPublisherConfig.ucVLanId;
      break;
      }

    case REG_GSPUB_VALNPRIO:
      {
      uRes = DevCfgForEdit.gsPublisherConfig.ucVLanPriority;
      break;
      }

    case REG_GSPUB_INTERVAL0:
    case REG_GSPUB_INTERVAL1:
    case REG_GSPUB_INTERVAL2:
    case REG_GSPUB_INTERVAL3:
      {
      uRes = DevCfgForEdit.gsPublisherConfig.uwIntevals[uRegNum - REG_GSPUB_INTERVAL0];
      break;
      }

#ifndef __vmSIMULATOR__
    case REG_GSPUB_ADDR1:
    case REG_GSPUB_ADDR2:
    case REG_GSPUB_ADDR3:
    case REG_GSPUB_ADDR4:
    case REG_GSPUB_ADDR5:
    case REG_GSPUB_ADDR6:
    case REG_GSPUB_ADDR7:
    case REG_GSPUB_ADDR8:
    case REG_GSPUB_ADDR9:
    case REG_GSPUB_ADDR10:
    case REG_GSPUB_ADDR11:
    case REG_GSPUB_ADDR12:
    case REG_GSPUB_ADDR13:
    case REG_GSPUB_ADDR14:
    case REG_GSPUB_ADDR15:
    case REG_GSPUB_ADDR16:
      {
      uRes = (uint32_t)(DevCfgForEdit.gsPublisherConfig.ucDestAddr[uRegNum - REG_GSPUB_ADDR1]);
      break;
      }
#endif

    // GOOSE Subscribers Config
    case REG_GSSUB1_APPID:
    case REG_GSSUB2_APPID:
    case REG_GSSUB3_APPID:
    case REG_GSSUB4_APPID:
    case REG_GSSUB5_APPID:
    case REG_GSSUB6_APPID:
    case REG_GSSUB7_APPID:
    case REG_GSSUB8_APPID:
    case REG_GSSUB9_APPID:
    case REG_GSSUB10_APPID:
    case REG_GSSUB11_APPID:
    case REG_GSSUB12_APPID:
    case REG_GSSUB13_APPID:
    case REG_GSSUB14_APPID:
    case REG_GSSUB15_APPID:
    case REG_GSSUB16_APPID:
      {
      uRes = DevCfgForEdit.gsSubscriberConfig[uRegNum - REG_GSSUB1_APPID].uwAppId;
      break;
      }

  #ifndef __vmSIMULATOR__
    case REG_GSSUB1_CONFIG:
    case REG_GSSUB2_CONFIG:
    case REG_GSSUB3_CONFIG:
    case REG_GSSUB4_CONFIG:
    case REG_GSSUB5_CONFIG:
    case REG_GSSUB6_CONFIG:
    case REG_GSSUB7_CONFIG:
    case REG_GSSUB8_CONFIG:
    case REG_GSSUB9_CONFIG:
    case REG_GSSUB10_CONFIG:
    case REG_GSSUB11_CONFIG:
    case REG_GSSUB12_CONFIG:
    case REG_GSSUB13_CONFIG:
    case REG_GSSUB14_CONFIG:
    case REG_GSSUB15_CONFIG:
    case REG_GSSUB16_CONFIG:
      {
      uRes = (uint32_t)(&DevCfgForEdit.gsSubscriberConfig[uRegNum - REG_GSSUB1_CONFIG]);
      break;
      }
  #endif
#endif

    default:
      {
      uRes = 0;

      break;
      }
    }

  return uRes;
}
//-----------------------------------------------------------------------------
// 写功能寄存器
static void _WriteFuncRegs(uint32_t uRegNum, uint32_t uValue)
{

  // 口令验证不需要权限
  switch( uRegNum )
    {
     case REG_PASSWORD:
      {
      if( uValue == DevConfig.Password
#ifdef SUPER_PASSWORD
          || SUPER_PASSWORD == uValue     // 后门
#endif
         )
        SetPassword1Ok;
      else
        ClrPassword1Ok;
      break;
      }

     case REG_PASSWORD2:
      {
      if( uValue == DevConfig.Password2
#ifdef SUPER_PASSWORD
          || SUPER_PASSWORD2 == uValue   // 后门
#endif
        )
        SetPassword2Ok;
      else
        ClrPassword2Ok;
      break;
      }

//     case REG_PASSWORDx:
//      {
//      // 验证态口令
//      if( DynaPswd_Verify( uValue >> 16, uValue & 0xFFFF ) == true )
//        SetPasswordxOk;
//      else
//        ClrPasswordxOk;

//      break;
//      }
    }

  // 功能寄存器基本需要使能才可写
  if( !GetEditEnable )
    return ;

  switch( uRegNum )
    {
    // Device Options
    case REG_FN_PROTECT_EN:
    case REG_FN_SOE_AUTORET:
    case REG_FN_CT_1A:
    case REG_FN_VOLTAGE_MODE:
      {
      uint32_t uMask = (1 << (uRegNum - REG_FN_DEV_OPTION));
      if( STATE_TRUE == uValue )
        DevCfgForEdit.Options |= uMask;
      else
        DevCfgForEdit.Options &= ~uMask;

      SetEditState( REM_CFG_MODIFIED );
      break;
      }

    // 设备参数
    // Devive Configs
    case REG_DEVFUNC:
      {
      // 只读
      break;
      }

    case REG_FN_DEVFUNC:
      {
#if NUM_DEV_FUNCs > 1
      if( DevCfgForEdit.DevFunc != uValue && NUM_DEV_FUNCs > uValue )
        {
        DevCfgForEdit.DevFunc = uValue;

        SetEditState( REM_FUN_MODIFIED );
        }
#endif
      break;
      }

#if CNT_Holdings > 0
    case REG_ACTV_HOLD_BLK:
      {
      // 只读
      break;
      }

    case REG_FN_ACTV_HOLD_BLK:
      {
        // 切换当前定值区
      DevIntf_ChangeActiveHoldingBlock( TOKEN_INTF_OPERATE, uValue );

      break;
      }

    case REG_EDIT_HOLD_BLK:
      {
      // 只读
      break;
      }

    // 切换编辑器
    case REG_FN_EDIT_HOLD_BLK:
      {
      DevIntf_ChangeEditHoldingBlock( TOKEN_INTF_OPERATE, uValue );

      break;
      }

    case REG_FN_COPY_HOLD_BLK:
      {
      DevIntf_HoldingCopyTo( TOKEN_INTF_OPERATE, uValue );

      break;
      }
#endif

//    case REG_DISTRIBUTOR:
//      {
//      // 只读
//      break;
//      }

//    case REG_FN_Distributor:
//      {
//      if( DISTBR_GetCount() > uValue )
//        {
//        if( DevCfgForEdit.Distributor != uValue )
//          {
//          DevCfgForEdit.Distributor = uValue;

//          SetEditState( REM_CFG_MODIFIED );
//          }
//        }
//      break;
//      }


    case REG_LANGUAGE:
      {
      break;
      }

    case REG_FN_Language:
      {
      const TDevRegInfoItem* pItem = DevIntf_GetRegInfo( REG_FN_Language );
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( nullptr == pItem, GFC_ErrRegNum );
#endif
      if( pItem->Max >= uValue )
        {
        DevCfgForEdit.Language = uValue;

        SetEditState( REM_CFG_MODIFIED );
        }
      break;
      }

    case REG_FN_PASSWORD:
      {
      if( DevCfgForEdit.Password != uValue )
        {
        DevCfgForEdit.Password = uValue;

        SetEditState( REM_CFG_MODIFIED );
        }
      break;
      }

    case REG_FN_PASSWORD2:
      {
      if( DevCfgForEdit.Password2 != uValue )
        {
        DevCfgForEdit.Password2 = uValue;

        SetEditState( REM_CFG_MODIFIED );
        }
      break;
      }

//    case REG_FN_PT_RATE:
//    //case REG_FN_PT2_RATE:
//    case REG_FN_CT_RATE:
//    //case REG_FN_CT2_RATE:
//      {
//      uRegNum = (uRegNum - REG_FN_PT_RATE) + REG_PT_RATE - REG_DEVCONFIG;
//      if( DevCfgForEdit.Configs[uRegNum] != uValue )
//        {
//        DevCfgForEdit.Configs[uRegNum] = uValue;
//        SetEditState( REM_CFG_MODIFIED );
//        }
//      break;
//      }

//    // 遥信消抖延时
//    case REG_FN_DI_JEDELAY:
//      {
//      if( DevCfgForEdit.Configs[REG_DI_JE_DELAY - REG_DEVCONFIG] != uValue )
//        {
//        DevCfgForEdit.Configs[REG_DI_JE_DELAY - REG_DEVCONFIG] = uValue;
//        SetEditState( REM_CFG_MODIFIED );
//        }
//      break;
//      }

//    // 遥控超时
//    case REG_FN_RC_OVERTIME:
//      {
//      if( DevCfgForEdit.Configs[REG_RC_OVERTIME - REG_DEVCONFIG] != uValue )
//        {
//        DevCfgForEdit.Configs[REG_RC_OVERTIME - REG_DEVCONFIG] = uValue;
//        SetEditState( REM_CFG_MODIFIED );
//        }
//      break;
//      }

#ifdef REG_LLCurr
    // 电流死区
    case REG_FN_LLCurr:
      {
      if( DevCfgForEdit.Configs[REG_LLCurr - REG_DEVCONFIG] != uValue )
        {
        DevCfgForEdit.Configs[REG_LLCurr - REG_DEVCONFIG] = uValue;

        SetEditState( REM_CFG_MODIFIED );
        }
      break;
      }
#endif

#ifdef REG_LLVolt
    // 电压死区
    case REG_FN_LLVolt:
      {
      if( DevCfgForEdit.Configs[REG_LLVolt - REG_DEVCONFIG] != uValue )
        {
        DevCfgForEdit.Configs[REG_LLVolt - REG_DEVCONFIG] = uValue;

        SetEditState( REM_CFG_MODIFIED );
        }
      break;
      }
#endif

//    // 直流配置
//    case REG_FN_DC0Min:
//    case REG_FN_DC0Max:
//    case REG_FN_DC1Min:
//    case REG_FN_DC1Max:
//    case REG_FN_DC2Min:
//    case REG_FN_DC2Max:
//    case REG_FN_DC3Type:
//    case REG_FN_DC3Min:
//    case REG_FN_DC3Max:
//    case REG_FN_DC4Type:
//    case REG_FN_DC4Min:
//    case REG_FN_DC4Max:
//    case REG_FN_DC5Type:
//    case REG_FN_DC5Min:
//    case REG_FN_DC5Max:
//      {
//      uRegNum = (uRegNum - REG_FN_DC0Min) + REG_DC0Min - REG_DEVCONFIG;
//      if( DevCfgForEdit.Configs[uRegNum] != uValue )
//        {
//        DevCfgForEdit.Configs[uRegNum] = uValue;
//        SetEditState( REM_CFG_MODIFIED );
//        }
//      break;
//      }

#ifndef __vmSIMULATOR__
    // 日期时间
    case REG_FN_DATETIME:
      {
      RTC_SetTime( &DevCache.DateTime );

      break;
      }

    case REG_DATE_YEAR:
      {
      DevCache.DateTime.Year = uValue;
      break;
      }

    case REG_DATE_MONTH:
      {
      DevCache.DateTime.Month = uValue;
      break;
      }

    case REG_DATE_DAY:
      {
      DevCache.DateTime.Day = uValue;
      break;
      }

    case REG_TIME_HOUR:
      {
      DevCache.DateTime.Hours = uValue;
      break;
      }

    case REG_TIME_SECOND:
      {
      DevCache.DateTime.Seconds = uValue;
      break;
      }

    case REG_TIME_MSECOND:
      {
      DevCache.DateTime.MilSeconds = uValue;
      break;
      }
#endif // 
      
//    // 以太网1地址
//    case REG_NIC1_CONFIG:
//      {
//      if( TOKEN_EDIT_SET == uValue )
//        {
//        // 保存配置，应用修改
//        memcpy( &GetNIC1Config, GetEditNIC1Cfg, sizeof(TNICConfig) );
//        FIX_SaveDevConfig( 0 );

//#ifndef __vmSIMULATOR__
//        //
//        netIntfApplyOption( 0 );
//#endif
//        SetEditState( REM_ETH1_MODIFIED );
//        SetSetHW( RSH_NIC1 );
//        }
//      else
//        {
//        // 恢复设设置
//        memcpy( GetEditNIC1Cfg, &GetNIC1Config, sizeof(TNICConfig) );
//        }

//      break;
//      }

//    case REG_NIC1_ADDR:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      if( *(uint32_t*)pNicConfig->Addr != uValue )
//        {
//        *(uint32_t*)pNicConfig->Addr = uValue;

//        // 2018.12.26 MAC地址随IP地址变化
//        pNicConfig->MAC[4] = pNicConfig->Addr[2];
//        pNicConfig->MAC[5] = pNicConfig->Addr[3];

//        SetEditState( REM_ETH1_MODIFIED );
//        SetSetHW( RSH_NIC1 );
//        }

//      break;
//      }

//    case REG_NIC1_ADDR1:
//    case REG_NIC1_ADDR2:
//    case REG_NIC1_ADDR3:
//    case REG_NIC1_ADDR4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;

//      // 如果 IP地址的最后一节与MAC地址的最后一节相同，(2018.12.26 MAC地址强制随IP地址变化)
//      // 则说 是随动状态，没有被单独修改
//      // 则继续随动
//      if( REG_NIC1_ADDR3 == uRegNum )
//        pNicConfig->MAC[4] = uValue;
//      else if( REG_NIC1_ADDR4 == uRegNum )
//        pNicConfig->MAC[5] = uValue;

//      pNicConfig->Addr[uRegNum - REG_NIC1_ADDR1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    case REG_NIC1_NetMask:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      *(uint32_t*)pNicConfig->Mask = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    case REG_NIC1_NetMask1:
//    case REG_NIC1_NetMask2:
//    case REG_NIC1_NetMask3:
//    case REG_NIC1_NetMask4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      pNicConfig->Mask[uRegNum - REG_NIC1_NetMask1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    case REG_NIC1_NetGate:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      *(uint32_t*)pNicConfig->Gate = uValue;
//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    case REG_NIC1_NetGate1:
//    case REG_NIC1_NetGate2:
//    case REG_NIC1_NetGate3:
//    case REG_NIC1_NetGate4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      pNicConfig->Gate[uRegNum - REG_NIC1_NetGate1] = uValue;
//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//#ifndef __vmSIMULATOR__
//    case REG_NIC1_NetMAC:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      memcpy( pNicConfig->MAC, (uint8_t*)uValue, 6 );
//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }
//#endif

//    case REG_NIC1_NetMAC1:
//    case REG_NIC1_NetMAC2:
//    case REG_NIC1_NetMAC3:
//    case REG_NIC1_NetMAC4:
//    case REG_NIC1_NetMAC5:
//    case REG_NIC1_NetMAC6:
//      {
//      TNICConfig* pNicConfig = GetEditNIC1Cfg;
//      pNicConfig->MAC[uRegNum - REG_NIC1_NetMAC1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    // 网络Socket
//    case REG_SKT11_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetHW( RSH_NIC1 );
//      break;
//      }

//    case REG_SKT11_Addr1:
//    case REG_SKT11_Addr2:
//    case REG_SKT11_Addr3:
//    case REG_SKT11_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      pSocketConfig->Addr[uRegNum - REG_SKT11_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET11 );
//      break;
//      }

//    case REG_SKT11_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET11 );
//      break;
//      }

//    case REG_SKT11_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET11 );
//      break;
//      }

//    case REG_SKT11_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(0);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET11 );
//      break;
//      }

//    // Socket 2
//    case REG_SKT12_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET12 );
//      break;
//      }

//    case REG_SKT12_Addr1:
//    case REG_SKT12_Addr2:
//    case REG_SKT12_Addr3:
//    case REG_SKT12_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      pSocketConfig->Addr[uRegNum - REG_SKT12_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET12 );
//      break;
//      }

//    case REG_SKT12_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET12 );
//      break;
//      }

//    case REG_SKT12_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET12 );
//      break;
//      }

//    case REG_SKT12_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(1);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET12 );
//      break;
//      }

//    // Socket 3
//    case REG_SKT13_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET13 );
//      break;
//      }

//    case REG_SKT13_Addr1:
//    case REG_SKT13_Addr2:
//    case REG_SKT13_Addr3:
//    case REG_SKT13_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      pSocketConfig->Addr[uRegNum - REG_SKT13_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET13 );
//      break;
//      }

//    case REG_SKT13_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET13 );
//      break;
//      }

//    case REG_SKT13_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET13 );
//      break;
//      }

//    case REG_SKT13_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(2);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET13 );
//      break;
//      }

//    // Socket 4
//    case REG_SKT14_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET14 );
//      break;
//      }

//    case REG_SKT14_Addr1:
//    case REG_SKT14_Addr2:
//    case REG_SKT14_Addr3:
//    case REG_SKT14_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      pSocketConfig->Addr[uRegNum - REG_SKT14_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET14 );
//      break;
//      }

//    case REG_SKT14_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET14 );
//      break;
//      }

//    case REG_SKT14_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET14 );
//      break;
//      }

//    case REG_SKT14_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(3);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET14 );
//      break;
//      }

//    // Socket 5
//    case REG_SKT15_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET15 );
//      break;
//      }

//    case REG_SKT15_Addr1:
//    case REG_SKT15_Addr2:
//    case REG_SKT15_Addr3:
//    case REG_SKT15_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      pSocketConfig->Addr[uRegNum - REG_SKT15_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET15 );
//      break;
//      }

//    case REG_SKT15_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET15 );
//      break;
//      }

//    case REG_SKT15_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET15 );
//      break;
//      }

//    case REG_SKT15_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(4);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET15 );
//      break;
//      }

//    // Socket 6
//    case REG_SKT16_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET16 );
//      break;
//      }

//    case REG_SKT16_Addr1:
//    case REG_SKT16_Addr2:
//    case REG_SKT16_Addr3:
//    case REG_SKT16_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      pSocketConfig->Addr[uRegNum - REG_SKT16_Addr1] = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET16 );
//      break;
//      }

//    case REG_SKT16_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET16 );
//      break;
//      }

//    case REG_SKT16_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET16 );
//      break;
//      }

//    case REG_SKT16_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC1SktCfg(5);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH1_MODIFIED );
//      SetSetSocket( RSS_SOCKET16 );
//      break;
//      }

//    // 以太网2地址
//    case REG_NIC2_CONFIG:
//      {
//      if( TOKEN_EDIT_SET == uValue )
//        {
//        // 保存配置，应用修改
//        memcpy( &GetNIC2Config, GetEditNIC2Cfg, sizeof(TNICConfig) );
//        FIX_SaveDevConfig( 1 );

//#ifndef __vmSIMULATOR__
//        //
//        netIntfApplyOption( 1 );
//#endif
//        SetEditState( REM_ETH2_MODIFIED );
//        SetSetHW( RSH_NIC2 );
//        }
//      else
//        {
//        // 恢复设设置
//        memcpy( GetEditNIC2Cfg, &GetNIC2Config, sizeof(TNICConfig) );
//        }

//      break;
//      }

//    case REG_NIC2_ADDR:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      if( *(uint32_t*)pNicConfig->Addr != uValue )
//        {
//        *(uint32_t*)pNicConfig->Addr = uValue;

//        SetEditState( REM_ETH2_MODIFIED );
//        SetSetHW( RSH_NIC2 );
//        }

//      break;
//      }

//    case REG_NIC2_ADDR1:
//    case REG_NIC2_ADDR2:
//    case REG_NIC2_ADDR3:
//    case REG_NIC2_ADDR4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;

//      // 如果 IP地址的最后一节与MAC地址的最后一节相同，
//      // 则说 是随动状态，没有被单独修改
//      // 则继续随动
//      if( REG_NIC2_ADDR4 == uRegNum &&
//          pNicConfig->Addr[3] == pNicConfig->MAC[5] )
//        pNicConfig->MAC[5] = uValue;

//      pNicConfig->Addr[uRegNum - REG_NIC2_ADDR1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    case REG_NIC2_NetMask:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      *(uint32_t*)pNicConfig->Mask = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    case REG_NIC2_NetMask1:
//    case REG_NIC2_NetMask2:
//    case REG_NIC2_NetMask3:
//    case REG_NIC2_NetMask4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      pNicConfig->Mask[uRegNum - REG_NIC2_NetMask1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    case REG_NIC2_NetGate:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      *(uint32_t*)pNicConfig->Gate = uValue;
//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    case REG_NIC2_NetGate1:
//    case REG_NIC2_NetGate2:
//    case REG_NIC2_NetGate3:
//    case REG_NIC2_NetGate4:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      pNicConfig->Gate[uRegNum - REG_NIC2_NetGate1] = uValue;
//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//#ifndef __vmSIMULATOR__
//    case REG_NIC2_NetMAC:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      memcpy( pNicConfig->MAC, (uint8_t*)uValue, 6 );
//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }
//#endif

//    case REG_NIC2_NetMAC1:
//    case REG_NIC2_NetMAC2:
//    case REG_NIC2_NetMAC3:
//    case REG_NIC2_NetMAC4:
//    case REG_NIC2_NetMAC5:
//    case REG_NIC2_NetMAC6:
//      {
//      TNICConfig* pNicConfig = GetEditNIC2Cfg;
//      pNicConfig->MAC[uRegNum - REG_NIC2_NetMAC1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    // 网络Socket
//    case REG_SKT21_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetHW( RSH_NIC2 );
//      break;
//      }

//    case REG_SKT21_Addr1:
//    case REG_SKT21_Addr2:
//    case REG_SKT21_Addr3:
//    case REG_SKT21_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      pSocketConfig->Addr[uRegNum - REG_SKT21_Addr1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET21 );
//      break;
//      }

//    case REG_SKT21_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET21 );
//      break;
//      }

//    case REG_SKT21_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET21 );
//      break;
//      }

//    case REG_SKT21_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(0);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET21 );
//      break;
//      }

//    // Socket 2
//    case REG_SKT22_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET22 );
//      break;
//      }

//    case REG_SKT22_Addr1:
//    case REG_SKT22_Addr2:
//    case REG_SKT22_Addr3:
//    case REG_SKT22_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      pSocketConfig->Addr[uRegNum - REG_SKT22_Addr1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET22 );
//      break;
//      }

//    case REG_SKT22_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET22 );
//      break;
//      }

//    case REG_SKT22_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET22 );
//      break;
//      }

//    case REG_SKT22_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(1);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET22 );
//      break;
//      }

//    // Socket 3
//    case REG_SKT23_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET23 );
//      break;
//      }

//    case REG_SKT23_Addr1:
//    case REG_SKT23_Addr2:
//    case REG_SKT23_Addr3:
//    case REG_SKT23_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      pSocketConfig->Addr[uRegNum - REG_SKT23_Addr1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET23 );
//      break;
//      }

//    case REG_SKT23_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET23 );
//      break;
//      }

//    case REG_SKT23_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET23 );
//      break;
//      }

//    case REG_SKT23_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(2);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET23 );
//      break;
//      }

//    // Socket 4
//    case REG_SKT24_Addr:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      *(uint32_t*)pSocketConfig->Addr = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET24 );
//      break;
//      }

//    case REG_SKT24_Addr1:
//    case REG_SKT24_Addr2:
//    case REG_SKT24_Addr3:
//    case REG_SKT24_Addr4:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      pSocketConfig->Addr[uRegNum - REG_SKT24_Addr1] = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET24 );
//      break;
//      }

//    case REG_SKT24_Mode:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      pSocketConfig->Mode = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET24 );
//      break;
//      }

//    case REG_SKT24_Port:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      pSocketConfig->Port = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET24 );
//      break;
//      }

//    case REG_SKT24_Protocol:
//      {
//      TNICSocketConfig* pSocketConfig = GetEditNIC2SktCfg(3);
//      pSocketConfig->Protocol = uValue;

//      SetEditState( REM_ETH2_MODIFIED );
//      SetSetSocket( RSS_SOCKET24 );
//      break;
//      }

#if NUM_UART_PORTS > 0
    // 串口1配置
    case REG_UART1_CFG:
      {
      if( TOKEN_EDIT_SET == uValue )
        {
        // 保存配置，应用修改
        memcpy( GET_UARTOPT(0), GetEditUART1Cfg(0), sizeof(TUARTConfig) );
        FIX_SaveDevConfig( 0 );

        //
#ifndef __vmSIMULATOR__
        MX_UART1_Init();
#endif

        SetEditState( REM_UART1_MODIFIED );
        SetSetHW( RSH_UART1 );
        }
      else
        {
        // 恢复设设置
        memcpy( GetEditUART1Cfg(0), GET_UARTOPT(0), sizeof(TUARTConfig) );
        }
      break;
      }

    case REG_UART1_ADDR:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      pUartConfig->Addr = uValue;

      SetEditState( REM_UART1_MODIFIED );
      SetSetHW( RSH_UART1 );
      break;
      }

    case REG_UART1_BAUDRATE:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      pUartConfig->Baudrate = uValue;

      SetEditState( REM_UART1_MODIFIED );
      SetSetHW( RSH_UART1 );
      break;
      }

    case REG_UART1_PARITY:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      pUartConfig->Parity = uValue;

      SetEditState( REM_UART1_MODIFIED );
      SetSetHW( RSH_UART1 );
      break;
      }

    case REG_UART1_PROTOCOL:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(0);
      pUartConfig->Protocol = uValue;

      SetEditState( REM_UART1_MODIFIED );
      SetSetHW( RSH_UART1_PROTOCOL );
      break;
      }
#endif

#if NUM_UART_PORTS > 1
    // 串口2配置
    case REG_UART2_CFG:
      {
      if( TOKEN_EDIT_SET == uValue )
        {
        // 保存配置，应用修改
        memcpy( GET_UARTOPT(0), GetEditUART1Cfg(1), sizeof(TUARTConfig) );
        FIX_SaveDevConfig( 0 );

        //
#ifndef __vmSIMULATOR__
        MX_UART3_Init();
#endif

        SetEditState( REM_UART2_MODIFIED );
        SetSetHW( RSH_UART2 );
        }
      else
        {
        // 恢复设设置
        memcpy( GetEditUART1Cfg(1), GET_UARTOPT(0), sizeof(TNICConfig) );
        }
      break;
      }

    case REG_UART2_ADDR:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      pUartConfig->Addr = uValue;

      SetEditState( REM_UART2_MODIFIED );
      SetSetHW( RSH_UART2 );
      break;
      }

    case REG_UART2_BAUDRATE:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      pUartConfig->Baud = uValue;

      SetEditState( REM_UART2_MODIFIED );
      SetSetHW( RSH_UART2 );
      break;
      }

    case REG_UART2_PARITY:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      pUartConfig->Parity = uValue;

      SetEditState( REM_UART2_MODIFIED );
      SetSetHW( RSH_UART2 );
      break;
      }

    case REG_UART2_PROTOCOL:
      {
      TUARTConfig* pUartConfig = GetEditUART1Cfg(1);
      pUartConfig->Protocol = uValue;

      SetEditState( REM_UART2_MODIFIED );
      SetSetHW( RSH_UART2_PROTOCOL );
      break;
      }
#endif

#ifdef USE_GOOSE
    // GOOSE Publisher
    case REG_GSPUB_APPID:
      {
      if( uValue != DevCfgForEdit.gsPublisherConfig.uwAppId )
        {
        DevCfgForEdit.gsPublisherConfig.uwAppId = uValue;

        SetEditState( REM_GSPUB_MODIFIED );
        SetSetHW( RSH_GOOSE_PUBLISHER );
        }

      break;
      }

    case REG_GSPUB_VALNID:
      {
      if( uValue != DevCfgForEdit.gsPublisherConfig.ucVLanId )
        {
        DevCfgForEdit.gsPublisherConfig.ucVLanId = uValue;

        SetEditState( REM_GSPUB_MODIFIED );
        SetSetHW( RSH_GOOSE_PUBLISHER );
        }

      break;
      }

    case REG_GSPUB_VALNPRIO:
      {
      if( uValue != DevCfgForEdit.gsPublisherConfig.ucVLanPriority )
        {
        DevCfgForEdit.gsPublisherConfig.ucVLanPriority = uValue;

        SetEditState( REM_GSPUB_MODIFIED );
        SetSetHW( RSH_GOOSE_PUBLISHER );
        }

      break;
      }

    case REG_GSPUB_INTERVAL0:
    case REG_GSPUB_INTERVAL1:
    case REG_GSPUB_INTERVAL2:
    case REG_GSPUB_INTERVAL3:
      {
      if( uValue != DevCfgForEdit.gsPublisherConfig.uwIntevals[uRegNum - REG_GSPUB_INTERVAL0] )
        {
        DevCfgForEdit.gsPublisherConfig.uwIntevals[uRegNum - REG_GSPUB_INTERVAL0] = uValue;

        SetEditState( REM_GSPUB_MODIFIED );
        SetSetHW( RSH_GOOSE_PUBLISHER );
        }
      break;
      }

  #ifndef __vmSIMULATOR__
    case REG_GSPUB_ADDR1:
    case REG_GSPUB_ADDR2:
    case REG_GSPUB_ADDR3:
    case REG_GSPUB_ADDR4:
    case REG_GSPUB_ADDR5:
    case REG_GSPUB_ADDR6:
    case REG_GSPUB_ADDR7:
    case REG_GSPUB_ADDR8:
    case REG_GSPUB_ADDR9:
    case REG_GSPUB_ADDR10:
    case REG_GSPUB_ADDR11:
    case REG_GSPUB_ADDR12:
    case REG_GSPUB_ADDR13:
    case REG_GSPUB_ADDR14:
    case REG_GSPUB_ADDR15:
    case REG_GSPUB_ADDR16:
      {
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( 0 == uValue, GFC_ErrParam );
#endif

      uint8_t *pAddr = DevCfgForEdit.gsPublisherConfig.ucDestAddr[uRegNum - REG_GSPUB_ADDR1];
      if( pAddr != (uint8_t*)uValue )
        {
        memcpy( pAddr, (uint8_t*)uValue, 6 );
        }

      SetEditState( REM_GSPUB_MODIFIED );
      SetSetHW( RSH_GOOSE_PUBLISHER );

      break;
      }
  #endif

    // GOOSE Subscribers Config
    case REG_GSSUB1_APPID:
    case REG_GSSUB2_APPID:
    case REG_GSSUB3_APPID:
    case REG_GSSUB4_APPID:
    case REG_GSSUB5_APPID:
    case REG_GSSUB6_APPID:
    case REG_GSSUB7_APPID:
    case REG_GSSUB8_APPID:
    case REG_GSSUB9_APPID:
    case REG_GSSUB10_APPID:
    case REG_GSSUB11_APPID:
    case REG_GSSUB12_APPID:
    case REG_GSSUB13_APPID:
    case REG_GSSUB14_APPID:
    case REG_GSSUB15_APPID:
    case REG_GSSUB16_APPID:
      {
      TGOOSESubscriberOption *pSubCfg = &DevCfgForEdit.gsSubscriberConfig[uRegNum - REG_GSSUB1_APPID];
      pSubCfg->uwAppId = uValue;

      SetEditState( REM_GSSUB_MODIFIED );
      SetSetHW( RSH_GOOSE_SUBSCRIBER );

      break;
      }

  #ifndef __vmSIMULATOR__
    case REG_GSSUB1_CONFIG:
    case REG_GSSUB2_CONFIG:
    case REG_GSSUB3_CONFIG:
    case REG_GSSUB4_CONFIG:
    case REG_GSSUB5_CONFIG:
    case REG_GSSUB6_CONFIG:
    case REG_GSSUB7_CONFIG:
    case REG_GSSUB8_CONFIG:
    case REG_GSSUB9_CONFIG:
    case REG_GSSUB10_CONFIG:
    case REG_GSSUB11_CONFIG:
    case REG_GSSUB12_CONFIG:
    case REG_GSSUB13_CONFIG:
    case REG_GSSUB14_CONFIG:
    case REG_GSSUB15_CONFIG:
    case REG_GSSUB16_CONFIG:
      {
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( 0 == uValue, GFC_ErrParam );
#endif
      TGOOSESubscriberOption *pSubCfg = &DevCfgForEdit.gsSubscriberConfig[uRegNum - REG_GSSUB1_CONFIG],
                             *pSubNew = (TGOOSESubscriberOption*)uValue;
      if( pSubCfg != pSubNew )
        memcpy( pSubCfg, pSubNew, sizeof(TGOOSESubscriberOption) );

      SetEditState( REM_GSSUB_MODIFIED );
      SetSetHW( RSH_GOOSE_SUBSCRIBER );

      break;
      }
  #endif
#endif

#ifdef USE_DEV_ASSERT
    default:
      {
      DEV_FAULT( GFC_ErrRegNum );

      break;
      }
#endif
    }
}
//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
// 初始化
void DevReg_Init(void)
{

  // 初始化缓冲区
  DevBuf_Init();

  // 设置边界
  DevReg_Write( REG_CRBorder, BORDER_SIGN_DW );
}
//-----------------------------------------------------------------------------
// 寄存器访问
//-----------------------------------------------------------------------------
// 读取寄存器
// 输入:
//  uReg:寄存器号
// 输出:无
// 返回:寄存器值
uint32_t DevReg_Read ( uint32_t uReg )
{

  uint32_t uRes{};
  switch( REG_TYPE(uReg) )
    {
    case REG_DEVOPTION:
      {
      uReg -= REG_DEVOPTION;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_DevOptBits, GFC_ErrRegNum );
#endif
      uint32_t uMask = (1 << uReg);
      if( 0 != (DevConfig.Options & uMask) )
        uRes = STATE_TRUE;
      else
        uRes = STATE_FALSE;

      break;
      }

    case REG_DEVCONFIG:
      {
      uRes = _GetDevCfgReg(uReg);

      break;
      }

#if CNT_DigInAlais > 0
    case REG_DIGINALAIS:
      {
      uReg -= REG_DIGINALAIS;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_DigInAlais, GFC_ErrRegNum );
#endif
      uRes = DevConfig.DigInAlais[uReg];

      break;
      }
#endif

    case REG_CALIBRATION:
      {
      uReg -= REG_CALIBRATION;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_Calibration, GFC_ErrRegNum );
#endif
      uRes = DevConfig.CaliCoef[uReg];

      break;
      }

#if CNT_Switchs > 0
    case REG_SWITCH:
      {
      uRes = _GetSwitchReg(uReg);

      break;
      }

    case REG_SWITCHEDIT:
      {
      uReg -= REG_SWITCHEDIT;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_Switchs, GFC_ErrRegNum );
#endif
      uRes = EditHolding.Data.Switch[uReg];

      break;
      }

    case REG_HOLDING:
      {
      uRes = _GetHoldingReg(uReg);

      break;
      }
#endif

#if CNT_Holdings > 0
    case REG_HOLDINGEDIT:
      {
      uReg -= REG_HOLDINGEDIT;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_Holdings, GFC_ErrRegNum );
#endif
      uRes = EditHolding.Data.Data[uReg];

      break;
      }
#endif

    case REG_DEVSTATE:
      {
      uReg -= REG_DEVSTATE;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= NUM_DEVICE_STATES, GFC_ErrRegNum );
#endif
      uRes = DevCache.DeviceState[uReg];
      break;
      }

    case REG_IOSTATE:
      {
      uReg -= REG_IOSTATE;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= NUM_IO_STATES, GFC_ErrRegNum );
#endif
      uRes = DevCache.IOState[uReg];
      break;
      }

    case REG_STATE:
      {
      uRes = _GetStateReg( uReg );

      break;
      }

    case REG_COMMON:
      {
      uRes = _GetCommonReg(uReg);

      break;
      }

    case REG_REAL:
      {
      uRes = (uint32_t)_GetRealReg(uReg);

      break;
      }

    case REG_FUNCTION:
      {
      uRes = _ReadFuncRegs(uReg);

      break;
      }

#ifdef USE_DEV_ASSERT
    default:
      {
      uRes = 0;
      DEV_ASSERT( true, GFC_ErrRegNum );

      break;
      }
#endif
    }

  return uRes;
}
//-----------------------------------------------------------------------------
// 写寄存器
// 输入:
//     uReg: 寄存器号
//  uwValue: 要写入寄存器的值
// 输出:
//   无
// 返回:
//   无
void DevReg_Write ( uint32_t uReg, uint32_t uValue )
{

  switch( REG_TYPE(uReg) )
    {
    case REG_DEVOPTION:
      {
      // 不允许直接写入

      break;
      }

    case REG_DEVCONFIG:
      {
      // 不允许从寄存器写
      break;
      }

    case REG_DIGINALAIS:
      {
      // 不允许从寄存器写
      break;
      }

    case REG_CALIBRATION:
      {
      // 不允许从寄存器写

      break;
      }

    case REG_SWITCH:
      {
      // 不允许从寄存器写

      break;
      }

#if CNT_Holdings > 0
    case REG_SWITCHEDIT:
      {
      uReg -= REG_SWITCHEDIT;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_Switchs, GFC_ErrRegNum );
#endif
      if( GetEditEnable && uValue != EditHolding.Data.Switch[uReg] )
        {
        if( uValue )
          EditHolding.Data.Switch[uReg] = STATE_TRUE;
        else
          EditHolding.Data.Switch[uReg] = STATE_FALSE;

        // 修改状态
        SetEditState( REM_SWT_MODIFIED );
        }

      break;
      }

    case REG_HOLDING:
      {
      // 不允许从寄存器写

      break;
      }

    case REG_HOLDINGEDIT:
      {
      uReg -= REG_HOLDINGEDIT;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uReg >= CNT_Holdings, GFC_ErrRegNum );
#endif
      if( GetEditEnable && uValue != EditHolding.Data.Data[uReg] )
        {
        EditHolding.Data.Data[uReg] = uValue;

        // 修改状态
        SetEditState( REM_HLD_MODIFIED );
        }

      break;
      }
#endif

    case REG_DEVSTATE:
      {
      // 不允许从寄存器修改
//      uReg -= REG_DEVSTATE;
//#ifdef USE_DEV_ASSERT
//      DEV_ASSERT( uReg >= NUM_DEVICE_STATES, GFC_ErrRegNum );
//      DEV_ASSERT( STATE_TRUE != uValue && STATE_FALSE != uValue,
//                  GFC_ErrParam );
//#endif
//      if( GetEditEnable )
//        DevCache.DeviceState[uReg] = uValue;
      break;
      }

    case REG_IOSTATE:
      {
      // 不允许从寄存器修改
//      uReg -= REG_IOSTATE;
//#ifdef USE_DEV_ASSERT
//      DEV_ASSERT( uReg >= NUM_IO_STATES, GFC_ErrRegNum );
//      DEV_ASSERT( STATE_TRUE != uValue && STATE_FALSE != uValue,
//                  GFC_ErrParam );
//#endif
//      if( GetEditEnable )
//        DevCache.IOState[uReg] = uValue;
      break;
      }

    case REG_STATE:
      {
      _SetStateReg( uReg, uValue );

      break;
      }

    case REG_COMMON:
      {
      _SetCommonReg( uReg, uValue );

      break;
      }

    case REG_REAL:
      {
      _SetRealReg( uReg, (float)uValue );

      break;
      }

    case REG_FUNCTION:
      {
      _WriteFuncRegs(uReg, uValue);

      break;
      }

#ifdef USE_DEV_ASSERT
    default:
      {
      DEV_ASSERT( true, GFC_ErrRegNum );

      break;
      }
#endif
    }
}
//-----------------------------------------------------------------------------
// 取指定类别的寄存器数量
// 输入：
//     uReg: 寄存器号
// 输出：无
// 返回：
//   寄存器数量
uint32_t DevReg_Count ( uint32_t uReg )
{


  uint32_t uRes;
  switch( REG_TYPE(uReg) )
    {
    case REG_DEVOPTION:
      {
      uRes = CNT_DevOptBits;
      break;
      }

    case REG_DEVCONFIG:
      {
      uRes = CNT_DevConfigs;
      break;
      }

#if CNT_DigInAlais > 0
    case REG_DIGINALAIS:
      {
      uRes = CNT_DigInAlais;
      break;
      }
#endif

    case REG_CALIBRATION:
      {
      uRes = CNT_Calibration;
      break;
      }

#if CNT_Switchs > 0
    case REG_SWITCH:
    case REG_SWITCHEDIT:
      {
      uRes = CNT_Switchs;
      break;
      }
#endif

#if CNT_Holdings > 0
    case REG_HOLDING:
    case REG_HOLDINGEDIT:
      {
      uRes = CNT_Holdings;
      break;
      }
#endif

    case REG_DEVSTATE:
      {
      uRes = NUM_DEVICE_STATES;
      break;
      }

    case REG_IOSTATE:
      {
      uRes = NUM_IO_STATES;
      break;
      }

    case REG_STATE:
      {
      uRes = NUM_STATE_REGS;
      break;
      }

    case REG_COMMON:
      {
      uRes = NUM_COMMON_REGS;
      break;
      }

    case REG_FUNCTION:
      {
          uRes = REG_INDEX((uint32_t)-1) + 1;
      break;
      }

    default:
      {
      uRes = 0;
      break;
      }
    }

  return uRes;
}
//-----------------------------------------------------------------------------
// 寄存器的快速访问方法
// 在调试状态( USE_DEV_ASSERT )下使用函数访问，否则用直接寄存器访问
//-----------------------------------------------------------------------------
#ifdef __REG_DEBUG
  // 取设置配置寄存器
  TDevStateReg _GetDevCfgReg(uint32_t reg)
  {
    TDevStateReg uwRes;
    if( REG_DEVCONFIG == REG_TYPE(reg) &&
        (reg - REG_DEVCONFIG) < CNT_DevConfigs )
      {
      uwRes = DevConfig.Configs[(reg) - REG_DEVCONFIG];
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      uwRes = 0;
      }

    return uwRes;
  }
  //---------------------------------------------------------------------------
  // 取压板寄存器
  TStateReg _GetSwitchReg(uint32_t reg)
  {
    TStateReg ucRes;
    if( REG_SWITCH == REG_TYPE(reg) &&
        (reg - REG_SWITCH) < CNT_Switchs )
      {
      ucRes = DevHolding.Data.Switch[(reg) - REG_SWITCH];
      if( STATE_TRUE != ucRes && STATE_FALSE != ucRes )
        DEV_FAULT( GFC_ErrValue );
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      ucRes = 0;
      }

    return ucRes;
  }
  //---------------------------------------------------------------------------
  // 取定值寄存器
  uint32_t _GetHoldingReg(uint32_t reg)
  {
    uint32_t uwRes;
    if( REG_HOLDING == REG_TYPE(reg) &&
        (reg - REG_HOLDING) < CNT_Holdings )
      {
      uwRes = DevHolding.Data.Data[(reg) - REG_HOLDING];
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      uwRes = 0;
      }

    return uwRes;
  }
  //---------------------------------------------------------------------------
  // 取IO状态
  TStateReg  _GetIOStateReg(uint32_t reg)
  {
    TStateReg ucRes;
    if( REG_IOSTATE == REG_TYPE(reg) &&
        (reg - REG_IOSTATE) < NUM_IO_STATES )
      {
      ucRes = DevCache.IOState[(reg) - REG_IOSTATE];
      if( STATE_TRUE != ucRes && STATE_FALSE != ucRes )
        DEV_FAULT( GFC_ErrValue );
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      ucRes = 0;
      }

    return ucRes;
  }
  //---------------------------------------------------------------------------
  // 取状态寄存器
  TStateReg _GetStateReg(uint32_t reg)
  {
    uint8_t ucRes;
    if( REG_STATE == REG_TYPE(reg) &&
        (reg - REG_STATE) < NUM_STATE_REGS )
      {
      ucRes = DevCache.State[(reg) - REG_STATE];
      if( STATE_TRUE != ucRes && STATE_FALSE != ucRes )
        DEV_FAULT( GFC_ErrValue );
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      ucRes = 0;
      }

    return ucRes;
  }
  //---------------------------------------------------------------------------
  // 取状态寄存器备份数据
  TStateReg _GetStateReg_bk(uint32_t reg)
  {
    uint8_t ucRes;
    if( REG_STATE == REG_TYPE(reg) &&
        (reg - REG_STATE) < NUM_STATE_REGS )
      {
      ucRes = DevCache.State_bk[(reg) - REG_STATE];
      if( STATE_TRUE != ucRes && STATE_FALSE != ucRes )
        DEV_FAULT( GFC_ErrValue );
      }
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      ucRes = 0;
      }

    return ucRes;
  }

  //---------------------------------------------------------------------------
  // 写状态寄存器
  void  _SetStateReg(uint32_t reg, TStateReg val)
  {
    if( REG_STATE == REG_TYPE(reg) &&
        (reg - REG_STATE) < NUM_STATE_REGS )
      {
      if( STATE_TRUE == val || STATE_FALSE == val )
        DevCache.State[(reg) - REG_STATE] = val;
      else
        DEV_FAULT( GFC_ErrValue );
      }
    else
      DEV_FAULT( GFC_ErrRegNum );
  }
  //---------------------------------------------------------------------------
  // 写状态寄存器备份区
  void  _SetStateReg_bk(uint32_t reg, TStateReg val)
  {
    if( REG_STATE == REG_TYPE(reg) &&
        (reg - REG_STATE) < NUM_STATE_REGS )
      {
      if( STATE_TRUE == val || STATE_FALSE == val )
        DevCache.State_bk[(reg) - REG_STATE] = val;
      else
        DEV_FAULT( GFC_ErrValue );
      }
    else
      DEV_FAULT( GFC_ErrRegNum );
  }

  //---------------------------------------------------------------------------
  // 取通用寄存器
  TCommonReg  _GetCommonReg(uint32_t reg)
  {
    uint32_t uRes;
    if( REG_COMMON == REG_TYPE(reg) &&
        (reg - REG_COMMON) < NUM_COMMON_REGS )
      uRes = DevCache.Common[(reg) - REG_COMMON];
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      uRes = 0;
      }

    return uRes;
  }
  //---------------------------------------------------------------------------
  // 设置通用寄存器
  void  _SetCommonReg(uint32_t reg, TCommonReg val)
  {
    if( REG_REAL == REG_TYPE(reg) &&
        (reg - REG_REAL) < NUM_COMMON_REGS )
      DevCache.Reals[(reg) - REG_REAL] = val;
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      }
  }
//-----------------------------------------------------------------------------
// 取浮点寄存器
TRealReg    _GetCommonReg(uint32_t reg)
{
}
//-----------------------------------------------------------------------------
// 设置浮点寄存器
void   _SetCommonReg(uint32_t reg, TRealReg val)
{
    TRealReg uRes;
    if( REG_REAL == REG_TYPE(reg) &&
        (reg - REG_REAL) < NUM_REAL_REGS )
      uRes = DevCache.Reals[(reg) - REG_REAL];
    else
      {
      DEV_FAULT( GFC_ErrRegNum );
      uRes = 0;
      }

    return uRes;
}
//-----------------------------------------------------------------------------
 #ifdef USE_GOOSE
 //-----------------------------------------------------------------------------
// // 读FA寄存器
// TGOOSEIInfo _GetFAStateReg(uint32_t sub, uint32_t idx)
// {
//
//   TGOOSEIInfo giRes;
//
//   if( NUM_SUBSCRIBERS > sub && FAC_MAX_GOOSEPDUS > idx )
//     giRes = DevCache.FAInfos[sub][idx];
//   else
//     {
//     giRes = STATE_FALSE;
//      DEV_FAULT( GFC_ErrRegNum );
//     }
//
//  return giRes;
// }
// //-----------------------------------------------------------------------------
// // 写FA寄存器
// void  _SetFAStateReg(uint8_t sub, uint32_t val)
// {
//
//   if( (NUM_SUBSCRIBERS >= sub) && ( sub > 0 ) )
//     {
//     DevCache.FAInfos[sub - 1] = val;
//     }
//   else
//     {
//     DEV_FAULT( GFC_ErrRegNum );
//     }
// }
 #endif

#endif // __REG_DEBUG
//-----------------------------------------------------------------------------
