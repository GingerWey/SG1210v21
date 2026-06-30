//-----------------------------------------------------------------------------
/*
 File        : GUIMisc.h
 Version     : V1.10
 By          : �����Ƽ�
 Description :�����������
        
 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "GUIMisc.h"
#include "GUI.h"
#include "LCD.h"

//=============================================================================
// ���غ�
//-----------------------------------------------------------------------------

//=============================================================================
// ���ݽṹ
//-----------------------------------------------------------------------------

//=============================================================================
// ���÷�������
//-----------------------------------------------------------------------------

//=============================================================================
// ��������
//-----------------------------------------------------------------------------
// �����������
static uint32_t prngState = 0;
//=============================================================================
// ���ط���
//-----------------------------------------------------------------------------

//=============================================================================
// ȫ�ַ���
//-----------------------------------------------------------------------------
// Alpha���������
void LineAlphaMix( uint32_t *puBkBuf, uint32_t *puFtBuf, 
                   uint32_t  uSize,   uint32_t  uAlpha )
{
  
  uint32_t uRevAlpha = 1024 - uAlpha;
  
  uint8_t *pucCrBk = (uint8_t*)puBkBuf,
          *pucCrFt = (uint8_t*)puFtBuf;
  for( int iIdx = uSize * 4 - 2; iIdx >= 0; iIdx-- )
    {
    if( 3 != (iIdx & 0x3) )
      {
      uint32_t uMixer;
      uMixer  = pucCrFt[iIdx] * uAlpha;
      uMixer += pucCrBk[iIdx] * uRevAlpha;
      uMixer >>= 10;
      pucCrBk[iIdx] = uMixer & 0xFF;
      }
    }  
}
//-----------------------------------------------------------------------------
// �������
void GWGradientFillLine( uint32_t *puBuf,    uint32_t uSize, 
                         uint32_t uBegColor, uint32_t uEndColor )
{

  int8_t *pucCrBeg = (int8_t*)&uBegColor;
//  pucCrEnd = (int8_t*)&uEndColor;

  int     iDif[3];
  iDif[0] = (int)(uEndColor & 0xFF) - (int)(uBegColor & 0xFF);
  iDif[1] = (int)((uEndColor >> 8) & 0xFF)  - (int)((uBegColor >> 8) & 0xFF);
  iDif[2] = (int)((uEndColor >> 16) & 0xFF) - (int)((uBegColor >> 16) & 0xFF);
  
  puBuf[0] = uBegColor;
  for( int iPix = 1; iPix < uSize; iPix++ )
    {
    int8_t *pucRes = (int8_t*)&puBuf[iPix];
      
    for( int iIdx = 2; iIdx >= 0; iIdx-- )
      {
      pucRes[iIdx] = pucCrBeg[iIdx] + iDif[iIdx] * (iPix + 1) / (int)uSize;
      }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void GWInitRand(uint32_t seed)
{

  //Seed the pseudo-random number generator
   prngState += seed;
}
//-----------------------------------------------------------------------------
uint32_t GWGetRand(void)
{
   uint32_t result;

  //Use a linear congruential generator (LCG) to update the state of the PRNG
  prngState *= 1103515245;
  prngState += 12345;
  result = (prngState >> 16) & 0x07FF;

  prngState *= 1103515245;
  prngState += 12345;
  result <<= 10;
  result |= (prngState >> 16) & 0x03FF;

  prngState *= 1103515245;
  prngState += 12345;
  result <<= 10;
  result |= (prngState >> 16) & 0x03FF;

  //Return the resulting value
  return result;
}
//-----------------------------------------------------------------------------
int32_t GWGetRandRange(int32_t min, int32_t max)
{
  
  uint32_t result;

  //Return a random value in the given range
  result = min + GWGetRand() % (max - min + 1);

  return result;
}
//-----------------------------------------------------------------------------
