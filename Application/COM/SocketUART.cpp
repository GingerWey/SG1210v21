//-----------------------------------------------------------------------------
/*
 File        : SocketUART.cpp
 Version     : V1.01
 By          : 银网科技

 Description :用于UART的Socket对象实现
        
 Date       : 2015.12.12
 
 2024.1.15
   v2.0
   代码规范
*/
//-----------------------------------------------------------------------------
#include "SocketUART.h"
#include "Protocol.h"

#include "UARTChls.h"
#include "DevRegs.h"

#include "cmsis_os.h"
#include <string.h>
//=============================================================================
// 局部宏
//-----------------------------------------------------------------------------

//=============================================================================
// 局部数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// 局部数据
//-----------------------------------------------------------------------------

//=============================================================================
// 公用类实现
//-----------------------------------------------------------------------------
// class TCOMSocketUART
//----------------------------------------------------------------------------
// pucBuffer: 接收缓冲区
//  uBufLen: pucBuffer的长度
// 返回：实际读写的字节数
uint32_t TCOMSocketUART::Read (void *pvBuffer, uint32_t uBufLen )
{

  // 
  if( nullptr == pvBuffer || NUM_UART_CHLs < m_uwIdent )
    return 0;
  
  uint8_t *pucBuffer = (uint8_t*)pvBuffer;
  
  // 取Socket对应的通道
  uint32_t uRevCnt = 0;
  TUARTCtrlBlock *pUartCtrl = &GetUARTChl(m_uwIdent);
  
  // 两个缓冲区都有报文？
  if( 0 < pUartCtrl->uwRevACnt && 0 < pUartCtrl->uwRevBCnt )
    {
    // 都有
    // 确定两个缓冲区接收的先后中次序
    if( pUartCtrl->uwOrderA < pUartCtrl->uwOrderB )
      {
      // A在前，B在后
      // 按次序读出
      memcpy( pucBuffer, pUartCtrl->ucRxBufA, pUartCtrl->uwRevACnt );
      uRevCnt = pUartCtrl->uwRevACnt;

      memcpy( pucBuffer + uRevCnt, pUartCtrl->ucRxBufB, pUartCtrl->uwRevBCnt );
      uRevCnt += pUartCtrl->uwRevBCnt;
      }
    else
      {
      // B在前，A在后
      // 按次序读出
      memcpy( pucBuffer, pUartCtrl->ucRxBufB, pUartCtrl->uwRevBCnt );
      uRevCnt = pUartCtrl->uwRevBCnt;

      memcpy( pucBuffer + uRevCnt, pUartCtrl->ucRxBufA, pUartCtrl->uwRevACnt );
      uRevCnt += pUartCtrl->uwRevACnt;
      }

    pUartCtrl->uwRevACnt = 0;
    pUartCtrl->uwRevBCnt = 0;
    }
    
  // A区有报文
  else if( 0 < pUartCtrl->uwRevACnt )
    {  
    // 读
    memcpy( pucBuffer, pUartCtrl->ucRxBufA, pUartCtrl->uwRevACnt );
    uRevCnt = pUartCtrl->uwRevACnt;

    pUartCtrl->uwRevACnt = 0;
    }      
  // A区有报文
  else if( 0 < pUartCtrl->uwRevBCnt )
    { 
    memcpy( pucBuffer, pUartCtrl->ucRxBufB, pUartCtrl->uwRevBCnt );
    uRevCnt = pUartCtrl->uwRevBCnt;

    pUartCtrl->uwRevBCnt = 0;
    }
                             
  // 清接收次序  
  pUartCtrl->uwOrderA  = 0;
  pUartCtrl->uwOrderB  = 0;

  // 收到报文
  if( uRevCnt > 0 )
    {
#ifdef REG_UART_ACTIVE
    SetUARTActive( RUA_UART1_Rx << m_uwIdent );
#endif
    }
    
  return uRevCnt;
}
//-----------------------------------------------------------------------------
uint32_t TCOMSocketUART::Write( const void *pvBuffer, 
                                   uint32_t uNumBytes,
                                   uint32_t uDestAddr,
                                   uint32_t uDestPort )
{
  
  HAL_StatusTypeDef hRes;
  if( sksSentOk == StateGet( sksSentOk ) )
    {
    if( nullptr != pvBuffer && 0 < uNumBytes && NUM_UART_CHLs > m_uwIdent )
      hRes = UartChl_Send( m_uwIdent, pvBuffer, uNumBytes );
    else 
      hRes = HAL_ERROR;
    }
  else
    hRes = HAL_BUSY;

  // 发送报文，点Tx灯
  if( HAL_OK == hRes && 0 < uNumBytes )
    {
    // 点Tx灯
//    SetCLEDState( RCLED_UART1_Tx << (m_uwIdent * 2) );

#ifdef REG_UART_ACTIVE
    SetUARTActive( RUA_UART1_Tx << m_uwIdent );
#endif
    }
  
  return (HAL_OK == hRes)? uNumBytes : 0;
}
//-----------------------------------------------------------------------------
// 取未接收的报文长度
uint32_t TCOMSocketUART::RevNumBytes(void)
{

  TUARTCtrlBlock *pUartCtrl = &GetUARTChl(m_uwIdent);

  uint32_t uNumBytes;
  if( 0 < pUartCtrl->uwRevACnt )
    {  
    uNumBytes = pUartCtrl->uwRevACnt;
    }
  else if( 0 < pUartCtrl->uwRevBCnt )
    {  
    uNumBytes = pUartCtrl->uwRevBCnt;
    }
  else
    uNumBytes = 0;

  return uNumBytes;
}
//-----------------------------------------------------------------------------
