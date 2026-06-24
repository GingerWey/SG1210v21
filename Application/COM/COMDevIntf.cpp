//-----------------------------------------------------------------------------
/*
 File        : COMDevIntf.cpp
 Version     : V1.01
 By          : 卫荣平

 Description :实现 通讯协议与系统之间的接口对象

 Date       : 2017.8.19
*/
//-----------------------------------------------------------------------------
#include "COMDevIntf.h"

#include "Dev_Cfg.h"
#include "DevRegs.h"
#include "DevRegInfo.h"
#include "DevIntf.h"
#include "DevFunc.h"

#include "DevDebug.h"

#include "RAMHeap.h"

#include "rtc.h"
//#include "Relay.h"

#if WAVELOGGER_EN > 0
 #include "WaveLogger.h"
#endif

#include <string.h>
//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------

//=============================================================================
// 本地数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// 全局变量引用
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 装置参数区的编辑区
extern TDeviceFixed DevCfgForEdit;
//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
//=============================================================================
// 全局对象
//-----------------------------------------------------------------------------
// TCommDevInterface
//  通讯协议与系统之间的接口 
//-----------------------------------------------------------------------------
// 定时事件
// 输入：
//   uTick: 时脉
// 输出：（无）
// 返回：（无）
void TCommDevInterface::OnTick(uint32_t uTick)
{

#ifdef __vmSIMULATOR__
  (void)( uTick );
#endif

#ifdef Remote_Timeout_ms
  // 判断是否 遥控超时
  if( m_uRemoteCtrlStart > 0 && 
      Remote_Timeout_ms < DevIntf_DiffTimeMs( m_uRemoteCtrlStart ) )
    RemotrCtrlCancel();
  
  // 判断是否 写入超时
  if( m_uRemoteServiceStart > 0 && 
      Remote_Timeout_ms < DevIntf_DiffTimeMs( m_uRemoteServiceStart ) )
    WriteCancel();
#endif
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           内存管理
//-----------------------------------------------------------------------------
// 分配置内存空间
// 输入：
//  uSize:  需要的内存空间的尺寸
// 输出： （无）
// 返回：
//   0: 分配失败   非0: 内存空间指针
void* TCommDevInterface::Malloc(uint32_t uSize)
{

  void* pResult;
  pResult = RAM_Malloc( uSize );

#ifdef USE_DEV_ASSERT
   DEV_ASSERT( nullptr == pResult, GFC_OutOfMem );
#endif

  return pResult;
}
//-----------------------------------------------------------------------------
// 释放内存空间
// 输入：
//  pvMem: 内存空间指针
// 输出： （无）
// 返回： （无）
void TCommDevInterface::MemFree(void* pvMem)
{

  RAM_Free( pvMem );
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           信息点表访问
//-----------------------------------------------------------------------------
// 取指定的信息点表
// 输入：
//  gType:  指定信息组的类型
// 输出：（无）
// 返回：
//   0 = 无指定类型的信息组   else 信息组指针
const TCommInfoGroup* TCommDevInterface::GetInfoGroup(TCommInfoGroupType gType)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == m_pInfoCluster, GFC_EmptyPtr  );
#endif
    
  const TCommInfoGroup* pGroup = nullptr;
  for( uint32_t uIdx = 0; uIdx < m_pInfoCluster->Count; uIdx++ )
    {
    if( m_pInfoCluster->Groups[uIdx]->Type == gType )
      {
      pGroup = m_pInfoCluster->Groups[uIdx];
      break;
      }
    }

  return pGroup;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           寄存器访问
//-----------------------------------------------------------------------------
// 读取寄存器的值
// 输入：
//  uRegNum:  寄存器号
//    Value:  要求填写  Value.type指定返回值的类型
// 输出：
//    Value:  读取的结果在 Value.Data中
// 返回：
//   true = 读成功   false = 读失败
bool TCommDevInterface::ReadReg(uint32_t uRegNum, TCOMValue &Value)
{

#ifdef USE_DEV_ASSERT
  // 粗判寄存器有效性
  DEV_ASSERT( 0 == REG_TYPE(uRegNum) || REG_TEST <= REG_TYPE(uRegNum), GFC_ErrParam );
#endif

  const TDevRegInfoItem* pRegInfo = DevIntf_GetRegInfo(uRegNum);
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pRegInfo, GFC_ErrRegNum );
#endif

  // 根据数据类型，摘取数据属性
  switch( SIT_GetType(pRegInfo->Property) )
    {
    case SIT_Type_Measure:
    case SIT_Type_Protect:
    case SIT_Type_Config:
    case SIT_Type_Holding:
      {
      // 填写取值范围 和 放大倍率
      Value.Scale = pRegInfo->Scale;

      // 电流定值是根据 CT来决定的
      if( SIT_MUL_20In == ( pRegInfo->Property & SIT_MUL_20In) )
        {
        if( GetDevOption(REG_CT_1A) )
          Value.Max = 20;
        else
          Value.Max = 100;
        }
      else
        {
        if( pRegInfo->Scale > 1 )
          Value.Max = (float)pRegInfo->Max / pRegInfo->Scale;
        else if( pRegInfo->Scale < -1 )
          Value.Max = (float)pRegInfo->Max * (-pRegInfo->Scale);
        else
          Value.Max = pRegInfo->Max;
        }
      break;
      }

    case SIT_Type_Metering:
      {
      // 填写取值范围 和 放大倍率
      Value.Scale = pRegInfo->Scale;
      Value.Max   = pRegInfo->Max;
      break;
      }

    default:
      {
      Value.Scale = 1;
      Value.Max   = 0;
      break;
      }
    }

  bool bRes = true;

  // 从运行区中取值
  uint32_t uValue = DevReg_Read(uRegNum);

  switch( Value.type )
    {
    case cvtDWord:
      {
      if( SIT_SIGNED == (pRegInfo->Property & SIT_SIGNED) )
        Value.Data.i32Value = (int32_t)uValue;
      else
        Value.Data.u32Value = uValue;
      break;
      }

    case cvtWord:
      {
      if( SIT_SIGNED == (pRegInfo->Property & SIT_SIGNED) )
        Value.Data.i16Value = (int16_t)uValue;
      else
        Value.Data.u16Value = (uint16_t)uValue;
      break;
      }

    case cvtByte:
      {
      if( SIT_SIGNED == (pRegInfo->Property & SIT_SIGNED) )
        Value.Data.i8Value = (int8_t)uValue;
      else
        Value.Data.u8Value = (uint8_t)uValue;
      break;
      }

    case cvtFloat:
      {
      if( SIT_SIGNED == (pRegInfo->Property & SIT_SIGNED) )
         Value.Data.f32Value = (float)((int)uValue);
      else
         Value.Data.f32Value = (float)(uValue);

      // 按浮点传实际值
      if( pRegInfo->Scale > 1 )
        Value.Data.f32Value = Value.Data.f32Value / pRegInfo->Scale;
      else if( pRegInfo->Scale < -1 )
        Value.Data.f32Value = Value.Data.f32Value * (-pRegInfo->Scale);

      break;
      }

    case cvtBool:
      {
      Value.Data.bValue = bool(uRegNum == STATE_TRUE);
      break;
      }

    default:
      {
      uValue = 0;
      bRes   = false;
      break;
      }
    }

  return bRes;
}
//-----------------------------------------------------------------------------
// 读取寄存器的值
// 输入：
// uInfAddr:  信息点号
//    Value:  要求填写Value.type,指定返回值的类型
// 输出：
//    Value:  读取的结果在 Value.Data中
// 返回：
//   true = 读成功   false = 读失败
bool TCommDevInterface::ReadInf(uint32_t uInfAddr, TCOMValue &Value)
{

  // 转成Reg
  uint32_t uRegNum = GetRegByInf( uInfAddr );
  if( 0 == uRegNum )
    return false;

  // 读回寄存器值
  return ReadReg( uRegNum, Value);
}
//-----------------------------------------------------------------------------
// 写到寄存器
// 输入：
//  uRegNum:  寄存器号
//    Value:  要写入的值
// 输出：（无）
// 返回：
//   true = 写成功   false = 写失败
bool TCommDevInterface::WriteReg(uint32_t uRegNum,  TCOMValue &Value)
{


  const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr );
#endif

  // 是有效的闭锁寄存器？  “远方就地”为【就地】状态时，不允许远程维护
  if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->LocalLatchReg))
    {
    // 闭锁寄存器 闭锁状态？
    if( STATE_TRUE != _GetIOStateReg( pIntf->RmtCtrlList->LocalLatchReg ) )
      return false;
    }
#ifdef Remote_Timeout_ms
  // 差别是否超时
  if( 0 == m_uRemoteServiceStart ||
      Remote_Timeout_ms <= DevIntf_DiffTimeMs( m_uRemoteServiceStart ) )
    {
    // 当写入超时时，放弃之前的修改
    if( 0 != DevIntf_GetServiceModfied() )
      DevIntf_ServiceCancel();

    // 准备 远程维护
    DevIntf_DevCfgEditPropare( TOKEN_INTF_OPERATE );

    // 记录本次维护的 起始时刻
    m_uRemoteServiceStart = DevIntf_GetTickCount();;
    }
#endif

    // 取寄存器描述
    const TDevRegInfoItem* pRegInfo = DevIntf_GetRegInfo(uRegNum);
#ifdef USE_DEV_ASSERT
    DEV_ASSERT( nullptr == pRegInfo, GFC_ErrRegNum );
#endif

  // 根据数据类型，摘取数据属性
  switch( SIT_GetType(pRegInfo->Property) )
    {
    case SIT_Type_Measure:
    case SIT_Type_Protect:
    case SIT_Type_Config:
    case SIT_Type_Holding:
      {
      // 填写取值范围 和 放大倍率
      Value.Scale = pRegInfo->Scale;

      // 电流定值是根据 CT来决定的
      if( SIT_MUL_20In == ( pRegInfo->Property & SIT_MUL_20In) )
        {
        if( GetDevOption(REG_CT_1A) )
          Value.Max = 20;
        else
          Value.Max = 100;
        }
      else
        {
        if( pRegInfo->Scale > 1 )
          Value.Max = (float)pRegInfo->Max * pRegInfo->Scale;
        else if( pRegInfo->Scale < -1 )
          Value.Max = (float)pRegInfo->Max / (-pRegInfo->Scale);
        else
          Value.Max = pRegInfo->Max;
        }
      break;
      }

    default:
      {
      Value.Scale = 1;
      Value.Max = 0;
      break;
      }
    }


  // 转换值
  uint32_t uValue;

  switch( Value.type )
    {
    case cvtDWord:
      {
      uValue = (uint32_t)Value.Data.i32Value;
      break;
      }

    case cvtWord:
      uValue = (uint32_t)Value.Data.i16Value;
      break;

    case cvtByte:
      uValue = (uint32_t)Value.Data.i8Value;
      break;

    case cvtFloat:
      {
      // 浮点数中传的是实际值，要应用寄存器的倍率
      if( pRegInfo->Scale > 1 )
        uValue = Value.Data.f32Value * pRegInfo->Scale + 0.5f;
      else if( pRegInfo->Scale < -1 )
        uValue = Value.Data.f32Value / (-pRegInfo->Scale) + 0.5f;
      else
        uValue = Value.Data.f32Value +0.5f;
      break;
      }

    case cvtBool:
      uValue = Value.Data.bValue? STATE_TRUE : STATE_FALSE;
      break;

    default:
      uValue = 0;
      break;
    }

  // 有效性判定
  if( cvtBool != Value.type && 0 < Value.Max )
    {
    // 超范围
    if( uValue > Value.Max )
      return false;
    }

  // 根据类型 填写
  bool bRes = false;
  switch( REG_TYPE(uRegNum) )
    {
    //----------------- 设备配置
    case REG_DEVOPTION:
      {
      uRegNum -= REG_DEVOPTION;
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uRegNum >= CNT_DevOptBits, GFC_ErrRegNum );
#endif
      uint32_t uMask = (1 << uRegNum);
      if( 0 != uValue && 0 == (DevCfgForEdit.Options & uMask) )
        {
        DevCfgForEdit.Options |= uMask;
        // 修改状态
        SetEditState( REM_CFG_MODIFIED );

        bRes = true;
        }
      else if( 0 == uValue && 0 != (DevCfgForEdit.Options & uMask) )
        {
        DevCfgForEdit.Options &= ~uMask;
        // 修改状态
        SetEditState( REM_CFG_MODIFIED );

        bRes = true;
        }

      break;
      }

    //----------------- 设备参数
    case REG_DEVCONFIG:
      {
      uRegNum -= REG_DEVCONFIG;
#ifdef USE_DEV_ASSERT
        DEV_ASSERT( uRegNum >= CNT_DevConfigs, GFC_ErrRegNum );
#endif
      if( uValue != DevCfgForEdit.Configs[uRegNum] )
        {
        // 填编辑区
        DevCfgForEdit.Configs[uRegNum] = uValue;

        // 修改状态
        SetEditState( REM_CFG_MODIFIED );

        bRes = true;
        }

      break;
      }

#ifdef CNT_Switchs
    //----------------- 保护压板
    case REG_SWITCH:
      uRegNum = uRegNum - REG_SWITCH + REG_SWITCHEDIT;
#ifdef __vmSIMULATOR__
  #ifdef USE_DEV_ASSERT
      DEV_ASSERT( uRegNum - REG_SWITCHEDIT >= CNT_Switchs, GFC_ErrRegNum );
  #endif

      // 允许维护
      SetEditEnable;

      // 写入寄存器
      DevReg_Write(uRegNum, uValue? STATE_TRUE : STATE_FALSE);

      // 禁止维护
      SetEditDisable;

      bRes = true;

      break;
#endif

    case REG_SWITCHEDIT:
      {
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uRegNum - REG_SWITCHEDIT >= CNT_Switchs, GFC_ErrRegNum );
#endif

      // 允许维护
      SetEditEnable;

      // 写入寄存器
      DevReg_Write(uRegNum, uValue? STATE_TRUE : STATE_FALSE);

      // 禁止维护
      SetEditDisable;

      bRes = true;

      break;
      }
#endif

#ifdef CNT_Holdings
    //----------------- 保护定值
    case REG_HOLDING:
      uRegNum = uRegNum - REG_HOLDING + REG_HOLDINGEDIT;
#ifdef __vmSIMULATOR__
  #ifdef USE_DEV_ASSERT
      DEV_ASSERT( uRegNum - REG_HOLDINGEDIT >= CNT_Holdings, GFC_ErrRegNum );
  #endif

      // 写入的定值要在允许的范围内
      if( pRegInfo->Max >= uValue && pRegInfo->Min <= (int32_t)uValue )
        {
        // 允许维护
        SetEditEnable;

        // 写入寄存器
        DevReg_Write( uRegNum, uValue );

        // 禁止维护
        SetEditDisable;

        bRes = true;
        }
      else
        bRes = false;

      break;
#endif
    case REG_HOLDINGEDIT:
      {
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( uRegNum - REG_HOLDINGEDIT >= CNT_Holdings, GFC_ErrRegNum );        
#endif

      // 写入的定值要在允许的范围内
      if( pRegInfo->Max >= uValue && pRegInfo->Min <= (int32_t)uValue )
        {
        // 允许维护
        SetEditEnable;

        // 写入寄存器
        DevReg_Write( uRegNum, uValue );

        // 禁止维护
        SetEditDisable;

        bRes = true;
        }
      else
        bRes = false;

      break;
      }
#endif

    //----------------- 状态寄存器
    case REG_STATE:
      {
      // 写寄存器
      DevReg_Write(uRegNum, uValue? STATE_TRUE : STATE_FALSE);

      break;
      }
    //----------------- 功能寄存器
    case REG_FUNCTION:
      {
      // 允许操作的功能寄存器罗列在下面
      switch( uRegNum )
        {
        // 切换定值区
        case REG_ACTV_HOLD_BLK:
          {
//          // 从通信切定值区，要求同时切换运行区和编辑区
//          if( 0 == (DevIntf_ChangeActiveHoldingBlock(TOKEN_INTF_OPERATE, uValue) |
//                    DevIntf_ChangeEditHoldingBlock(TOKEN_INTF_OPERATE, uValue) ) )
//            {
//            //
//            SetGUIState( RGUI_HLDBLKCHED );
//            bRes = true;
//            }
          DevCfgForEdit.ActiveHoldingBlock = uValue;
          SetEditState( REM_CFG_MODIFIED );
          bRes = true;
          break;
          }

        default:
          {
//          // 允许维护
//          SetEditEnable;
//
//          // 写寄存器
//          DevReg_Write(uRegNum, uValue);
//
//          // 禁止维护
//          SetEditDisable;
//
//          bRes = true;
          bRes = false;    // 其它寄存器不允许写

          break;
          }
        }

      break;
      }

    default:
      {
      bRes = false;
      break;
      }
    }

  return bRes;
}
//-----------------------------------------------------------------------------
// 写到寄存器
// 输入：
// uInfAddr:  信息点号
//    Value:  要写入的值
// 输出：（无）
// 返回：
//   true = 写成功   false = 写失败
bool TCommDevInterface::WriteInf(uint32_t uInfAddr, TCOMValue &Value)
{

  // 转成Reg
  uint32_t uRegNum = GetRegByInf( uInfAddr );
  if( 0 == uRegNum )
    return false;

  // 按Reg写入
  return WriteReg( uRegNum, Value);
}
//-----------------------------------------------------------------------------
// 写确认
// 输入：
//   uToken:  口令 必须是 CDI_TOKEN
// 输出：（无）
// 返回：
//   true = 写成功   false = 写失败
bool TCommDevInterface::WriteComfirm(uint32_t uToken)
{

  const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr );
#endif

  // 是有效的闭锁寄存器？  “远方就地”为【就地】状态时，不允许远程维护
  if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->LocalLatchReg))
    {
    // 闭锁寄存器 闭锁状态？
    if( STATE_TRUE != _GetIOStateReg( pIntf->RmtCtrlList->LocalLatchReg ) )
      return false;
    }

  // 审查令牌
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( CDI_TOKEN != uToken, GFC_ErrToken );
#else
  if( CDI_TOKEN != uToken )
    return false;
#endif

#ifdef Remote_Timeout_ms
  // 差别是否超时
  if( 0 == m_uRemoteServiceStart ||
      Remote_Timeout_ms <= DevIntf_DiffTimeMs( m_uRemoteServiceStart ) )
    {
    m_uRemoteServiceStart = 0;

    // 取消各种修改
    DevIntf_ServiceCancel();

    return false;
    }
#endif

#ifdef CNT_Holdings
  bool bRes;
  if( 0 == DevIntf_GetServiceModfied() )
    {
    bRes = false;
    }
  else
    {
    if( DevCfgForEdit.ActiveHoldingBlock != DevConfig.ActiveHoldingBlock )
      {
      uint32_t uBlockNo = DevCfgForEdit.ActiveHoldingBlock;
      // 从通信切定值区，要求同时切换运行区和编辑区
      if( 0 == (DevIntf_ChangeActiveHoldingBlock(TOKEN_INTF_OPERATE, uBlockNo) |
                DevIntf_ChangeEditHoldingBlock(TOKEN_INTF_OPERATE, uBlockNo) ) )
        {
        //
        SetGUIState( RGS_HLDBLKCHED );
        }
      }

    // 应用远程修改
    DevIntf_ServiceAccept();

//    // 要求配置外设
//    if( 0 != GetSetHW( RSH_NIC1 | RSH_UART1 | RSH_UART2 |
//                       RSH_UART1_PROTOCOL   | RSH_UART2_PROTOCOL ) )
//      DevIntf_PeripheralsReconfig();

    bRes = true;

    // 记录远程维护 事件
    //EVTMGR_AppendEvent( REG_EM_CONFIG, STATE_TRUE );
    }

  m_uRemoteServiceStart = 0;

  return bRes;
#else
  return false;
#endif
}
//-----------------------------------------------------------------------------
// 写放弃
// 输入：（无）
// 输出：（无）
// 返回：
//   true = 写成功   false = 写失败
bool TCommDevInterface::WriteCancel(void)
{

  if( 0 != DevIntf_GetServiceModfied() )
    DevIntf_ServiceCancel();

  return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           事件相关
//-----------------------------------------------------------------------------
// 获取最新的事件数量
// 输入：（无）
// 输出：（无）
// 返回：
//   0: 无新事件   >0: 新事件数量
uint32_t TCommDevInterface::GetNewEventCount(void)
{

  return EVTMGR_GetNewEventCount( m_EventFilter );
}
//-----------------------------------------------------------------------------
// 搜索最新的事件
// 输入：（无）
// 输出：（无）
// 返回：
//   0: 无新事件   非0: 新事件指针
const TCOMEventItem* TCommDevInterface::FetchEvent(void)
{

  const TEventLogItem* pEvtItem = EVTMGR_Fetch( m_EventFilter );
  if( nullptr == pEvtItem )
    return nullptr;

  // ------------- 搜索INF --------------
  // 寄存器地址
  uint32_t uRegNum = pEvtItem->Summary.State.RegNum;
  const TCommInfoItem* pInfItem = GetInfoByRegNum(uRegNum);
  if( nullptr != pInfItem )
    m_Event.InfoAddr = pInfItem->InfoAddr;
  else
    //m_Event.InfoAddr = 0;
    return nullptr;

  // ------------- 填写事件信息 --------------
  const TDevRegInfoItem* pRegInfo = DevIntf_GetRegInfo(uRegNum);
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pRegInfo, GFC_ErrRegNum );
#endif

  // 定值上的事件，是操作记录
  if( REG_DEVOPTION  == REG_TYPE(uRegNum) ||
      REG_DEVCONFIG  == REG_TYPE(uRegNum) ||
      REG_DIGINALAIS == REG_TYPE(uRegNum) ||
      REG_SWITCH     == REG_TYPE(uRegNum) ||
      REG_HOLDING    == REG_TYPE(uRegNum) )
    return FetchOpLog(pRegInfo, pEvtItem);
  
  const TEventProperty* pEvtProp = DevIntf_GetEvtProp( pRegInfo->Event );
#ifdef USE_DEV_ASSERT     
  DEV_ASSERT( nullptr == pEvtProp, GFC_EmptyPtr );
#endif

  // 事件摘要
  memcpy( &m_Event.Summary, &pEvtItem->Summary, sizeof(TEventLogSummary) );
  // 现场数据个数
  m_Event.NbrOfData = pEvtProp->NbrOfData;

  // 逐个读取现场数据，并转换为实际值
  for( uint32_t uIdx = 0; uIdx < pEvtProp->NbrOfData; uIdx++ )
    {
    if( REG_COMMON == REG_TYPE(pEvtProp->FieldDataReg[uIdx]) )
      {
      pRegInfo = DevIntf_GetRegInfo(pEvtProp->FieldDataReg[uIdx]);
      if( nullptr != pRegInfo )
        {
        uint32_t uValue = pEvtItem->FieldData[uIdx];
        if( pRegInfo->Scale > 1 )
          m_Event.Data[uIdx] = (float)uValue / pRegInfo->Scale;
        else if( pRegInfo->Scale < -1 )
          m_Event.Data[uIdx] = (float)uValue * (-pRegInfo->Scale);
        else
          m_Event.Data[uIdx] = uValue;
        }
      else
        {
#ifdef USE_DEV_ASSERT
        DEV_FAULT( GFC_EmptyPtr );
#endif
        m_Event.Data[uIdx] = 0;
        }
      }
    else
      {
#ifdef USE_DEV_ASSERT
      DEV_FAULT( GFC_ErrParam );
#endif
      m_Event.Data[uIdx] = 0;
      }
    }

  return &m_Event;
}
//-----------------------------------------------------------------------------
// 读操作记录
//   pRegInfo: 寄存器描述
// 返回：
//   0: 无新事件   非0: 新事件指针
const TCOMEventItem* TCommDevInterface::FetchOpLog(const TDevRegInfoItem* pRegInfo,
                                                   const TEventLogItem*   pEvtItem)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pRegInfo || nullptr == pEvtItem,  GFC_ErrParam );
#endif

  // 事件摘要
  memcpy( &m_Event.Summary, &pEvtItem->Summary, sizeof(TEventLogSummary) );
  // 现场数据个数
  m_Event.NbrOfData = 1;

  uint32_t uValue = pEvtItem->FieldData[0];
  if( pRegInfo->Scale > 1 )
    m_Event.Data[0] = (float)uValue / pRegInfo->Scale;
  else if( pRegInfo->Scale < -1 )
    m_Event.Data[0] = (float)uValue * (-pRegInfo->Scale);
  else
    m_Event.Data[0] = uValue;

  return &m_Event;
}
//-----------------------------------------------------------------------------
// 取事件的属性
// 事件的属性包括：动作名称、现场数据个数及现场数据寄存器号
// 输入:
//   uRegNum: 信息点号
// 输出：(无)
// 返回：
//   0 = 不可识别的属性  else 指定的事件处理属性
const TEventProperty* TCommDevInterface::GetEventProp(uint32_t uRegNum)
{

  const TDevRegInfoItem* pRegInfo = DevIntf_GetRegInfo( uRegNum );
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pRegInfo, GFC_ErrRegNum );
#endif
  
  return DevIntf_GetEvtProp( pRegInfo->Event );
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           录波相关
//-----------------------------------------------------------------------------
#if WAVELOGGER_EN > 0
//-----------------------------------------------------------------------------
// 获取录波记录数量
// 输入：（无）
// 输出：（无）
// 返回：
//   0: 无新事件   >0: 记录数量
uint32_t TCommDevInterface::GetWavelogCount(void)
{

  return WAVLOG_GetLogCount();
}
//-----------------------------------------------------------------------------
// 读录波事件摘要
// 输入：
//   uIndex: 录波记录的序号 0 ~ GetWavelogCount - 1
//           按时间Z-Order：最新的是0，次新的是1，......
// 输出：（无）
// 返回：
//   0: 指定的录波记录不存在   非0: 新录波记录指针
const TCOMWaveLogItem* TCommDevInterface::ReadWavelogSummary(uint32_t uIndex)
{

  // 读录波记录摘要到缓冲区
  if( WAVLOG_GetLogSummary( &m_WaveLog.Summary, uIndex ) != 0 )
    return nullptr;

  // ------------- 搜索INF --------------
  // 寄存器地址
  uint32_t uRegNum = m_WaveLog.Summary.State.RegNum;
  const TCommInfoItem* pInfItem = GetInfoByRegNum(uRegNum);
  if( 0 != pInfItem )
    m_WaveLog.InfoAddr = pInfItem->InfoAddr;
  else
    m_WaveLog.InfoAddr = 0;

  // ------------ 录波数据信息 ----------
  m_WaveLog.NbrOfSamples = WAVELOG_SAMPLES_PER_TRACK;
  // 零时刻：Summary.State.Type里保存的是触发前预录周波数
  m_WaveLog.PosOfTrigger = m_WaveLog.Summary.State.Type *
                           ADC_SAMPLES_PER_PEROID;

  return &m_WaveLog;
}
//-----------------------------------------------------------------------------
// 读取录波波形数据
// 输入：
//       uIndex: 录波记录的序号 0 ~ GetWavelogCount - 1
//               按时间Z-Order：最新的是0，次新的是1，......
//      uChlIdx: 录波的通道序号
//               模拟通道：0 ~ TCOMWaveLogItem.NbrOfMeasChannels - 1
//    uPosition: 起始位置，按采样点计算
//               模拟通道，每采样点2Bytes
//               数字通道，每采样点1Bit，按8bits对齐
//        pBuff: 缓冲区指针，波形数据将保存到该缓冲区
// uSampsToRead：要读回的采样点数
//               模拟通道：读回2 x uSampsToRead字节
//               数字通道：读回((uSampsToRead + 7) / 8)字节
// 输出：
//   pBuff：读回的波形数据
// 返回：
//   0: 参数错误   非0: 读回的字节数量
uint32_t TCommDevInterface::ReadWavelogChlData(uint32_t uIndex,
                                               uint32_t uChlIdx,
                                               uint32_t uPosition,
                                               void*    pBuff,
                                               uint32_t uSampsToRead)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pBuff || 0 == uSampsToRead, GFC_ErrParam );
#endif

  return WAVLOG_ReadTrackData( uIndex, uChlIdx, uPosition, pBuff, uSampsToRead );
}
//-----------------------------------------------------------------------------
#endif //WAVELOGGER_EN > 0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           遥控相关
//-----------------------------------------------------------------------------
// 遥控选择
// 不支持多重遥控，新的选择将覆盖之前的选择操作
// 输入：
//     uToken: 令牌 必须是 CDI_TOKEN
//   uInfAddr: 信息点号
// 输出：（无）
// 返回：
//   true = 成功   false = 失败
bool TCommDevInterface::RemotrCtrlSelect (uint32_t uToken, uint32_t uInfAddr)
{

  // 审查令牌
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( CDI_TOKEN != uToken, GFC_ErrToken );
#else
  if( CDI_TOKEN != uToken )
    return false;
#endif

   const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr );
#endif

  // 是有效的闭锁寄存器？ “远方就地”为【就地】状态时，不允许遥控
  if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->LocalLatchReg))
    {
    // 闭锁寄存器 闭锁状态？
    if( STATE_TRUE != _GetIOStateReg( pIntf->RmtCtrlList->LocalLatchReg ) )
      return false;
    }

  // 搜索Inf指定的寄存器
  uint32_t uRegNum = GetRegByInf( uInfAddr );
  //if( 0 == uRegNum || m_uRemoteCtrlReg != uRegNum )
  if( 0 == uRegNum  )
    return false;

  // 记录 选择的目标 和 起始时刻
  m_uRemoteCtrlStart = DevIntf_GetTickCount();
  m_uRemoteCtrlReg   = uRegNum;

  return true;
}
//-----------------------------------------------------------------------------
// 遥控执行
// 输入：
//   uToken:  令牌 必须是 CDI_TOKEN
// 输出：（无）
// 返回：
//   true = 成功   false = 失败
bool TCommDevInterface::RemotrCtrlExecute(uint32_t uToken)
{

  // 审查令牌
#ifdef USE_DEV_ASSERT      
  DEV_ASSERT( CDI_TOKEN != uToken, GFC_ErrToken );
#else
  if( CDI_TOKEN != uToken )
    return false;
#endif

  const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr);
#endif

  // 是有效的闭锁寄存器？ “远方就地”为【就地】状态时，不允许遥控
  if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->LocalLatchReg))
    {
    // 闭锁寄存器 闭锁状态？
    if( STATE_TRUE != _GetIOStateReg( pIntf->RmtCtrlList->LocalLatchReg ) )
      return false;
    }
  else
    return false;

  // 是否已“选择”
  if( 0 == m_uRemoteCtrlStart || 0 == m_uRemoteCtrlReg )
    return false;

#ifdef Remote_Timeout_ms
  // 判断是否超时
  bool bRes;
  uint32_t uTimeDiff = DevIntf_DiffTimeMs( m_uRemoteCtrlStart );
  if( Remote_Timeout_ms < uTimeDiff )
    {
    bRes = false;   // 超时
    }
  else// 执行遥控操作
    {
    _SetStateReg(m_uRemoteCtrlReg,STATE_TRUE);
    bRes = true;
    }
//  else if( REG_ST_RemoteCtrl0_Active <= m_uRemoteCtrlReg &&
//           REG_ST_RemoteCtrl9_Active >= m_uRemoteCtrlReg )
//    {
//    // 发送 继电器驱动指令
//    if( 0 == Relay_RemoteCtrlTrigger( RELAYON_TOKEN(m_uRemoteCtrlReg - REG_RELAY0) ) )
//      bRes = true;
//    else
//      bRes = false;
//    }
//  else
//    bRes = false;

  // 清遥控状态
  m_uRemoteCtrlStart = 0;
  m_uRemoteCtrlReg   = 0;

  return bRes;
#else
  return false;
#endif
}
//-----------------------------------------------------------------------------
// 遥控撤消
// 输入：
//   uInfAddr:  信息点号
// 输出：（无）
// 返回：
//   true = 成功   false = 失败
bool TCommDevInterface::RemotrCtrlCancel (void)
{

  bool bRes;
  if( 0 == m_uRemoteCtrlStart || 0 == m_uRemoteCtrlReg )
    bRes = false;
  else
    bRes = true;

  m_uRemoteCtrlStart = 0;
  m_uRemoteCtrlReg   = 0;

  return bRes;
}
//-----------------------------------------------------------------------------
// 远程复归
// 输入：
//   uToken:  令牌 必须是 CDI_TOKEN
// 输出：（无）
// 返回：
//   true = 成功   false = 失败
bool TCommDevInterface::RemoteSignalReturn(uint32_t uToken)
{

  // 审查令牌
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( CDI_TOKEN != uToken, GFC_ErrToken );
#else
  if( CDI_TOKEN != uToken )
    return false;
#endif

#ifdef TOKEN_RELAY_OFF
  // 信号复归
  bool bRes = Relay_SignalReturn(TOKEN_RELAY_OFF);
#else
  bool bRes = true;
#endif
  if( true == bRes )
    {
    // 关【事故】灯
    // ClrMLEDState( RML_MLED_FAULT );

    // 清全部持续状态
    ClrPersisent( -1u );

    // 更新灯、保存状态
    SetTodoTask( RTT_UPDATE_LED | RTT_SAVE_STATE );
    }

  // 记录远程复归 事件
  //EVTMGR_AppendEvent( REG_ER_SGLRET, STATE_TRUE );

  return bRes;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           装置相关
//-----------------------------------------------------------------------------
// 校时
// 输入：
//   DateTime: 时间日期
// 输出：（无）
// 返回：
//   true = 成功   false = 失败
bool TCommDevInterface::SetDateTime(const TDateTimeType &DateTime)
{

#ifdef RHF_IRIGB_MIS
  // 2020.11.28 Wey.
  // B码校时正常时，忽略网络校时
  if( 0 == GetHWFault( RHF_IRIGB_MIS ) )
    return true;
#endif

  bool bRes;
  // 检查 时间有效性
  if( DateTime.Year  >= YEAR_BEGIN &&
      DateTime.Year  <= YEAR_BEGIN + 100 &&
      DateTime.Month >  0    &&
      DateTime.Month <= 12   &&
      DateTime.Day   >  0    &&
      DateTime.Day   <= 31   &&
      DateTime.Hours      < 24 &&
      DateTime.Minutes    < 60 &&
      DateTime.Seconds    < 60 &&
      DateTime.MilSeconds < 1000 )
    {
    // 设置到 RTC
    if( 0 == RTC_SetTime( &DateTime ) )
      {
      bRes = true;

      // 记录远程校时 事件
      EVTMGR_AppendEvent( REG_EM_SET_RTC, STATE_TRUE );
      }
    else
      bRes = false;
    }
  else
    bRes = false;

  return bRes;
}
//-----------------------------------------------------------------------------
// 获取装置工作状态
// 输入：
//   dsType: 状态类型
// 输出：（无）
// 返回：
//   true = 置位   false = 无
bool TCommDevInterface::GetDevState( TCOMDevState dsType )
{

  bool bRes;
  switch( dsType )
    {
    case cdsDevNormal:
      {
      if( RDM_NORMAL == GetDevMode )
        bRes = true;
      else
        bRes = false;

      break;
      }

    case cdsHWNormal:
      {
      if( 0 == GetHWFault( -1u) )
        bRes = true;
      else
        bRes = false;

      break;
      }

    case cdsCFGNormal:
      {
      if( 0 == GetFXState(RFE_DCFG_ERROR | RFE_CLIB_ERROR |
                          RFE_HOLD_ERROR | REF_LOGIDX_ERR) )
        bRes = true;
      else
        bRes = false;

      break;
      }

    case cdsLocal:
      {
      const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr );
#endif

      // 是有效的闭锁寄存器？  “远方就地”为【就地】状态时，不允许远程维护
      if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->LocalLatchReg))
        {
        // 闭锁寄存器 闭锁状态？
        if( STATE_TRUE == _GetIOStateReg( pIntf->RmtCtrlList->LocalLatchReg ) )
          bRes = true;
        else
          bRes = false;
        }
      else
        bRes = false;

      break;
      }

    case cdsService:
      {
      const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
#ifdef USE_DEV_ASSERT
      DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->RmtCtrlList, GFC_EmptyPtr );
#endif

      // 是有效的闭锁寄存器？  “远方就地”为【就地】状态时，不允许远程维护
      if(  REG_IOSTATE == REG_TYPE(pIntf->RmtCtrlList->ServiceReg))
        {
        // 闭锁寄存器 闭锁状态？
        if( STATE_TRUE == _GetIOStateReg( pIntf->RmtCtrlList->ServiceReg ) )
          bRes = true;
        else
          bRes = false;
        }
      else
        bRes = false;

      break;
      }

    default:
      bRes = false;
      break;
    }

  return bRes;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           点号访问
//-----------------------------------------------------------------------------
// 用信息点号探索信息点
// 输入：
//   uInf: 信息点
// 输出：（无）
// 返回：
//   0: 失败   非0: 信息点描述指针
const TCommInfoItem* TCommDevInterface::GetInfoByInf(uint32_t uInf)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == m_pInfoCluster, GFC_EmptyPtr );
#endif

  const TCommInfoItem* pItem = nullptr;
  for( uint32_t uGIdx = 0; uGIdx < m_pInfoCluster->Count; uGIdx++ )
    {
    const TCommInfoGroup* pGroup = m_pInfoCluster->Groups[uGIdx];
    if( NULL==pGroup )
      continue;
    if( pGroup->InfBegin > uInf || pGroup->InfEnd < uInf )
      continue;

    // 二分法搜索
    uint32_t uBeg = 0, uEnd = pGroup->Count - 1,
             uPos = (uInf - pGroup->InfBegin);
    if( uPos > uEnd )
      uPos = (uBeg + uEnd) >> 1;

    const TCommInfoItem *pItems = pGroup->Items;
    while( uEnd - uBeg > 1 )
      {
      if( pItems[uPos].InfoAddr == uInf )
        {
        pItem = &pItems[uPos];
        break;
        }
      else if( pItems[uPos].InfoAddr > uInf )
        uEnd = uPos;
      else
        uBeg = uPos;

      uPos = (uBeg + uEnd) >> 1;
      }

    // 判定边界
    if( nullptr == pItem )
      {
      if( pItems[uPos].InfoAddr == uInf )
        pItem = &pItems[uPos];
      else if( pItems[uEnd].InfoAddr == uInf )
        pItem = &pItems[uEnd];
      else if( pItems[uBeg].InfoAddr == uInf )
        pItem = &pItems[uBeg];
      }

    // 搜索结束
    if( 0 != pItem )
      break;
    }
    
#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pItem, GFC_ErrRegNum );
#endif

  return pItem;
}
//-----------------------------------------------------------------------------
// 用信息点号探索寄存器
// 输入：
//   uInf: 信息点
// 输出：（无）
// 返回：
//   0:搜索失败  >0:寄存器地址
uint32_t TCommDevInterface::GetRegByInf(uint32_t uInf)
{

  const TCommInfoItem* pInfo = GetInfoByInf( uInf );
  if( nullptr == pInfo )
    return 0;

  return pInfo->RegNum;
}
//-----------------------------------------------------------------------------
// 用寄存器号探索通信信息点号
// 输入：
//   uRegNum: 寄存器号
// 输出：（无）
// 返回：
//   0: 失败   非0: 信息点描述指针
const TCommInfoItem* TCommDevInterface::GetInfoByRegNum(uint32_t uRegNum)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == m_pInfoCluster, GFC_EmptyPtr );
#endif

  const TCommInfoItem* pItem = nullptr;

  TCommInfoGroupType gType = SIG_UNKNOWN;
  switch( REG_TYPE(uRegNum) )
    {
    case REG_DEVOPTION:
      {
      gType = SIG_DevOption;
      break;
       }
    case REG_DEVCONFIG:
      {
      gType = SIG_DevConfig;
      break;
       }
    case REG_DIGINALAIS:
      {
      return nullptr;
       }
    case REG_CALIBRATION:
      {
      gType = SIG_Calibration;
      break;
       }
    case REG_SWITCH:
      {
      gType = SIG_VDI;
      break;
       }
    case REG_HOLDING:
      {
      gType = SIG_SetPoint;
      break;
       }
    case REG_DEVSTATE:
      {
      return nullptr;
       }
    case REG_IOSTATE:
      {
      if( REG_RelayState > uRegNum )
        gType = SIG_BinInput;
      else
        gType = SIG_Coils;
      break;
       }
    case REG_STATE:
      {
      gType = SIG_Event;
      break;
      }
    case REG_COMMON:
      {
      pItem = GetInfoByRegNum( GetInfoGroup(SIG_ProMeasure), uRegNum );
      if( nullptr == pItem )
        pItem = GetInfoByRegNum( GetInfoGroup(SIG_Measure), uRegNum );
      if( nullptr == pItem )
        pItem = GetInfoByRegNum( GetInfoGroup(SIG_Metering), uRegNum );
      break;
      }
    case REG_EVENT:
      {
      gType = SIG_Event;
      break;
      }
    default:
      {
      return nullptr;
      }
    }

  if( nullptr == pItem && SIG_UNKNOWN != gType )
    pItem = GetInfoByRegNum( GetInfoGroup(gType), uRegNum );

  return pItem;
}
//-----------------------------------------------------------------------------
// 在指定的组中搜索寄存器
// 输入：
//   pgInfo： 信息组指针
//   uRegNum: 寄存器号
// 输出：（无）
// 返回：
//   0: 失败   非0: 信息点描述指针
const TCommInfoItem* TCommDevInterface::GetInfoByRegNum(
                                               const TCommInfoGroup* pgInfo,
                                                            uint32_t uRegNum)
{

#ifdef USE_DEV_ASSERT
  DEV_ASSERT( nullptr == pgInfo, GFC_EmptyPtr );
#endif

  const TCommInfoItem* pItem = nullptr;
  for( uint32_t uIdx = 0; uIdx < pgInfo->Count; uIdx++)
    {
    if( uRegNum == pgInfo->Items[uIdx].RegNum )
      {
      pItem = &pgInfo->Items[uIdx];
      break;
      }
    }

  return pItem;
}
//-----------------------------------------------------------------------------
// 初始化接口
// 输入：（无）
// 输出：（无）
// 返回：（无）
void TCommDevInterface::InitIntf(void)
{

//#if WAVELOGGER_EN > 0
//  // ============== 填写录波信息 =================
//  // 模拟通道描述
//  // 取采样通道定义
//  const TDevFuncInterface* pIntf = DevFunc_CurFuncInterface();
//#ifdef USE_DEV_ASSERT
//  DEV_ASSERT( nullptr == pIntf || nullptr == pIntf->ADCRanks, GFC_ErrParam );
//#endif

//  m_WaveLog.NbrOfMeasChannels = NUM_ADC_CHANNELS;
//  for( uint32_t uIdx = 0; uIdx < NUM_ADC_CHANNELS; uIdx++ )
//    {
//    m_WaveLog.RegsOfMeas[uIdx] = pIntf->ADCRanks->RefRegNum[uIdx];
//    }

//  // 开入通道描述
//  m_WaveLog.NbrOfDIChannels = NUM_DigInputs;
//  for( uint32_t uIdx = 0; uIdx < NUM_DigInputs; uIdx++ )
//    {
//    m_WaveLog.RegsOfDI[uIdx] = REG_DI0 + uIdx;
//    }

//  // 开出通道描述
//  m_WaveLog.NbrOfDOChannels = NUM_Relays;
//  for( uint32_t uIdx = 0; uIdx < NUM_Relays; uIdx++ )
//    {
//    m_WaveLog.RegsOfDO[uIdx] = REG_RELAY0 + uIdx;
//    }
//#endif
}
//-----------------------------------------------------------------------------
