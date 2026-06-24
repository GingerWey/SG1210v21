//-----------------------------------------------------------------------------
/*
 File        : GPMainForm.cpp
 Version     : V1.10
 By          : 银网科技
 Description : 主窗体
        
 Date        : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "GPMainForm.h"

#include "GUI.h"
#include "GUICntr.h"
#include "GUIConf.h"
#include "GUIMisc.h"
#include "GUIPicture.h"
#include "PictureRes.h"
#include "Strings/TextStrs.h"

#include "FontCHS16LTH.h"
#include "FontCHS24LTH.h"
#include "FontCHS40HT.h"

#include "DevRegs.h"
#include "DevFixed.h"
#include "RamHeap.h"

#include "rtc.h"
#include "RNGen.h"
#include "gpio.h"
#include "BoardCtrl.h"

#include <stdio.h>
#include <string.h>

#include <arm_math.h>
//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------
//
#define PAGES_IN_FORM       1
#define PAGE1_PANELS        4
//-----------------------------------------------------------------------------
#define HIGHT_CAPTION       24

#define WIDTH_EDGE          4

#define HiAAF               4

// Colors
#define crFormBkg           GUI_MAKE_COLOR(0x001F1F1F)  
#define crFrameHigh         GUI_MAKE_COLOR(0x003F3F3F)  
#define crFrameDrak         GUI_MAKE_COLOR(0x000F0F0F)

// Caption
#define Caption_x1          (0)
#define Caption_y1          (0)
#define Caption_x2          (DESKTOP_WIDTH - 1)
#define Caption_y2          (HIGHT_CAPTION)

#define CapLabel_x1         (Caption_x1 + WIDTH_EDGE)
#define CapLabel_y1         (Caption_y1 + WIDTH_EDGE)
#define CapLabel_x2         (Caption_x2 - WIDTH_EDGE)
#define CapLabel_y2         (Caption_y2 - WIDTH_EDGE)

#define crCaptionFont       GUI_GRAY_AA  
#define ftCaption           GUI_FONT_16LTH_CHN

#define crCapFrameHi        GUI_GRAY_50
#define crCapFrameDark      crFormBkg

// Clock
#define rtClock_x1          (CapLabel_x1 + WIDTH_EDGE)
#define rtClock_y1          (CapLabel_y1)
#define rtClock_x2          (CapLabel_x1 + 128)
#define rtClock_y2          (CapLabel_y2)

#define crClockFont         GUI_GRAY_AA
#define ftClockFont         GUI_FONT_16B_ASCII

// 
#define rtCommadr_x1        (CapLabel_x2 - 32)
#define rtCommadr_y1        (CapLabel_y1)
#define rtCommadr_x2        (CapLabel_x2)
#define rtCommadr_y2        (CapLabel_y2)

// Addr
#define crCommAddrFont      GUI_GRAY_AA
#define ftCommAddrFont      GUI_FONT_16B_ASCII

// Panels Area
#define rtPanelsArea_x1     (6)
#define rtPanelsArea_y1     (Caption_y2 + WIDTH_EDGE)
#define rtPanelsArea_x2     (112)
#define rtPanelsArea_y2     (DESKTOP_HEIGHT - 10)

#define Num_Panels          4
#define Height_Panel        ((rtPanelsArea_y2 - rtPanelsArea_y1 + 1) / Num_Panels)
#define Height_Panel_Gap    (Height_Panel / 5)

#define rtPanelsIcon_x1     (rtPanelsArea_x1 + WIDTH_EDGE)
#define rtPanelsIcon_y1(r)  (rtPanelsArea_y1 + Height_Panel * (r) + Height_Panel_Gap)
#define rtPanelsIcon_x2     (rtPanelsIcon_x1 + 23)
#define rtPanelsIcon_y2(r)  (rtPanelsIcon_y1(r) + 35)

#define crPanelsLabelFont   GUI_GRAY_AA
#define ftPanelsLabelFont   GUI_FONT_16LTH_CHN

#define rtPanelsText_x1     (rtPanelsArea_x1 + 36)
#define rtPanelsText_y1(r)  (rtPanelsIcon_y1(r))
#define rtPanelsText_x2     (rtPanelsArea_x2 - 4)
#define rtPanelsText_y2(r)  (rtPanelsIcon_y2(r))

#define crPanelFrameHi      crFrameHigh//GUI_MAKE_COLOR(0x00301010)
#define crPanelFrameDark    crFormBkg

// Meteor
#define meteor_r            (90 * HiAAF)
#define meteor_w            (meteor_r * 2)
#define meteor_h            (meteor_r * 171 / 100)
#define meteor_cx           (220 * HiAAF)
#define meteor_cy           (108 * HiAAF)
#define meteor_as1          (-45.0)
#define meteor_ae1          (225.0)
#define meteor_as2          (210.0)
#define meteor_ae2          (-30.0)
#define meteor_IndDeg(p)    (meteor_as2  - (meteor_as2 - meteor_ae2) * (p))
#define meteor_IndAng(p)    ((meteor_as2 - (meteor_as2 - meteor_ae2) * (p)) * 3.14159265f / 180.0f)
#define metRad(dif)         (meteor_r - (dif) * HiAAF)

const GUI_POINT ptIndicator[] =
{
  { 40,  3 }, { 76,  2 }, { 80, 0 },
  { 76, -2 }, { 40, -3 }
};
#define NUM_Indicators      (sizeof(ptIndicator) / sizeof(ptIndicator[0]))

#define crMeteorFrameHi     GUI_LIGHTYELLOW
#define crMeteorFrameDark   GUI_BLACK
#define crMeteorDial        GUI_ORANGE
#define ftMeteorDial        GUI_FONT_6X8
#define crMeteorDialLine    GUI_GRAY_3F
#define crMeteorDialDark1   GUI_MAKE_COLOR(0xFF00324D)
#define crMeteorDialDark2   GUI_MAKE_COLOR(0xFF004970)
#define crMeteorDialDark3   GUI_MAKE_COLOR(0xFF006DA8)
#define crMeteorNumDial     GUI_MAKE_COLOR(0xFF1F1818)
#define crMeteorTextDial    GUI_GRAY_AA
#define crMeteorIndicator   GUI_RED
#define crMeteorShaftDark   GUI_GRAY_50
#define crMeteorShaftText   GUI_GRAY_9A
#define ftMeteorShaftText   GUI_FONT_32B_ASCII

// Voltage Value
#define rtValue_x1          ((meteor_cx - meteor_r)/ HiAAF)
#define rtValue_y1          ((meteor_cy + meteor_r * 9 / 10) / HiAAF)
#define rtValue_x2          ((meteor_cx + meteor_r)/ HiAAF)
#define rtValue_y2          (rtPanelsArea_y2 - WIDTH_EDGE)

#define ftVoltageFont       GUI_FONT_D32
#define crVoltageFont       GUI_LIGHTCYAN
#define ftVoltageLost       GUI_FONT_40LTH_CHN
#define crVoltageLost       GUI_RED
//=============================================================================
// 本地数据结构
//-----------------------------------------------------------------------------
typedef struct tagFormState
{
  uint32_t     uNextTick,
               uMsgIdx;
  uint32_t     uPageIdx;

  // Panels的状态
  uint32_t     uPage1PanelsState[PAGE1_PANELS];
  
  // 电压表相关
  struct tagMeteor {
    GUI_MEMDEV_Handle hMeteorMem;
    TCommonReg        uLastVoltage;    // 当前电压表上的值
    uint32_t          uUpLimit;        // 电压表上限
    uint32_t          uSections;       // 电压表刻度分段
    }          meteor;
  
  // 电压值
  TCommonReg   uVoltageValue;
  uint32_t     uVoltageTick;
} TMainFormState;
//=============================================================================
// 本地数据
//-----------------------------------------------------------------------------
static TMainFormState  *FpFormState = nullptr;

#define INTEVAL_UPDATE   1000
//=============================================================================
// 本地方法
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Page 1
//-----------------------------------------------------------------------------
static void _updateCaption1( bool bAll )
{

  GUI_SetTextMode( GUI_TEXTMODE_NORMAL );

  char szStr[32];
  if( true == bAll )
    {
    GUI_SetPenShape( GUI_PS_SQUARE );
    GUI_SetLineStyle( GUI_LS_SOLID );

    GUI_AA_EnableHiRes();
    GUI_AA_SetFactor( HiAAF );  // AAl因子

    // 左上角，时间
    GUI_DrawGradientH( Caption_x1,  rtClock_y2 - 1,
                       rtClock_x2,  rtClock_y2,
                       crCapFrameDark,   crCapFrameHi );
    GUI_SetColor( crCapFrameHi );
    GUI_SetPenSize( 2 );
    GUI_AA_DrawArc ( HiAAF * rtClock_x2, 
                     HiAAF * (rtClock_y2 - 10),
                     HiAAF * 10, HiAAF * 10,
                     -90, -45 );
    GUI_AA_DrawLine( HiAAF * (rtClock_x2 + 7), 
                     HiAAF * (rtClock_y2 - 4),
                     HiAAF * (rtClock_x2 + 7 + (HIGHT_CAPTION - (Caption_y2 - 7))),
                     0 );

    // 右上角，装置通信地址
    GUI_AA_DrawLine( HiAAF * (rtCommadr_x1 - 7), 
                     HiAAF * (rtCommadr_y2 - 4),
                     HiAAF * (rtCommadr_x1 - 7) - (HIGHT_CAPTION - (Caption_y2 - 7)),
                     0 );
    GUI_AA_DrawArc ( HiAAF * rtCommadr_x1,
                     HiAAF * (rtCommadr_y2 - 10),
                     HiAAF * 10, HiAAF * 10,
                     -135, -90 );
    GUI_DrawGradientH( rtCommadr_x1, rtCommadr_y2 - 1,
                       Caption_x2,   rtCommadr_y2,
                       crCapFrameHi, crCapFrameDark );

    GUI_AA_DisableHiRes();
    
    GUI_SetBkColor( crFormBkg );
    GUI_SetFont ( ftCommAddrFont );
    GUI_SetColor( crCommAddrFont );
    
    // 装置通信地址
    GUI_RECT aRect = { rtCommadr_x1 + WIDTH_EDGE, rtCommadr_y1 - 2, 
                       rtCommadr_x2,              rtCommadr_y2 - 2 };
    sprintf( szStr, "%d", GET_UARTOPT(0)->Addr );
    GUI_DispStringInRect( szStr, &aRect, GUI_TA_CENTER | GUI_TA_VCENTER );
    }

  // --------------- 实时时间
  TDateTimeType dtNow;
  RTC_GetTime( &dtNow );
  sprintf( szStr, "%04u-%02u-%02u %02u:%02u:%02u", 
           dtNow.Year, dtNow.Month, dtNow.Day, 
           dtNow.Hours, dtNow.Minutes, dtNow.Seconds );
  GUI_SetColor( crClockFont );
  GUI_SetFont ( ftClockFont );

  GUI_RECT cRect = { rtClock_x1, rtClock_y1 - 2, 
                     rtClock_x2, rtClock_y2 - 2 };
  GUI_DispStringInRect( szStr, &cRect, GUI_TA_LEFT | GUI_TA_VCENTER );
}
//-----------------------------------------------------------------------------
static void _updatePanels( bool bAll )
{

  GUI_SetColor( crPanelsLabelFont );
  GUI_SetFont ( ftPanelsLabelFont );
  GUI_SetTextMode( GUI_TEXTMODE_NORMAL );

  char szStr[20];
  GUI_RECT Rect = { rtPanelsText_x1, rtPanelsText_y1(0),
                    rtPanelsText_x2, rtPanelsText_y1(1) }; 
  
  // ---------------- 线圈状态
  int iValue = BoardCtrl_GetPassby();
  if( true == bAll ||
      iValue != FpFormState->uPage1PanelsState[0] )
    {
    uint32_t uStrId = 0;
    if( 0 == iValue )
      {
      GUI_DrawPicture( &picCoilsCYDK22x36jpg, 
                      rtPanelsIcon_x1, rtPanelsIcon_y1(0) );
#ifdef REG_PASSBY_EN
      if( 0 < _GetDevCfgReg(REG_PASSBY_EN) )
        //sprintf( szStr, "旁路" );
        uStrId = idMainFmSt11;
      else
#endif
        // sprintf( szStr, "脱扣动作" );
        uStrId = idMainFmSt12;
      }
    else
      {
      GUI_DrawPicture( &picCoilsCYHI22x36jpg, 
                      rtPanelsIcon_x1, rtPanelsIcon_y1(0) );
      //sprintf( szStr, "正常吸合" );
      uStrId = idMainFmSt13;
      }

    const char* pcStr = GetMultiLangString( uStrId );
    if( nullptr != pcStr )
      {
      GUI_DispStringInRect( pcStr, &Rect, GUI_TA_LEFT | GUI_TA_TOP );
      }

    const TGUIPicture *pPicture;
    if( 0 == (iValue & 0x01) )
      {
#ifdef REG_PASSBY_EN
      if( 0 < _GetDevCfgReg(REG_PASSBY_EN) )
        pPicture = &picLEDOangeOn16x16bmp;
      else
#endif
        pPicture = &picLEDGreenOn16x16bmp;
      }
    else
      pPicture = &picLEDRedOn16x16bmp;
    GUI_DrawPicture( pPicture, 
#if ENABLE_COIL2 > 0
                     rtPanelsText_x1 + 8, 
#else
                     rtPanelsText_x1 + 24, 
#endif
                     rtPanelsIcon_y2(0) - 16 );

#if ENABLE_COIL2 > 0
    if( 0 == (iValue & 0x02) )
      {
#ifdef REG_PASSBY_EN
      if( 0 < _GetDevCfgReg(REG_PASSBY_EN) )
        pPicture = &picLEDGreenOn16x16bmp;
      else
#endif
        pPicture = &picLEDOangeOn16x16bmp;
      }
    else
      pPicture = &picLEDRedOn16x16bmp;
    GUI_DrawPicture( pPicture, 
                     rtPanelsText_x1 + 32, rtPanelsIcon_y2(0) - 16 );
#endif

    FpFormState->uPage1PanelsState[0] = iValue;
    }

  Rect.y0 = rtPanelsText_y1(1);
  Rect.y1 = rtPanelsText_y2(1);

  // ---------------- 输出电压
  iValue = BoardCtrl_GetInvertorVoltage() | (GetCoilState() << 16);
  if( true == bAll ||
      iValue != FpFormState->uPage1PanelsState[1] )
    {
    FpFormState->uPage1PanelsState[1] = iValue;

    iValue &= 0xFFFF;

    const char* szState = nullptr;
    switch( GetCoilState() )
      {
      case COIL_Stop:
        szState = GetMultiLangString(idMainFmSt21); //"关闭";
        break;
      case COIL_Normal:
        szState = GetMultiLangString(idMainFmSt22); //"正常\n%0.2fV"
        sprintf( szStr, szState, (float)iValue / RATIO_Voltage);
        szState = szStr;
        break;
      case COIL_KeppOn:
        szState = GetMultiLangString(idMainFmSt23); //"保持\n%0.2fV"
        sprintf( szStr, szState, (float)iValue / RATIO_Voltage);
        szState = szStr;
        break;
      case COIL_KeppOff:
        szState = GetMultiLangString(idMainFmSt24); //"闭锁";
        break;
      case COIL_PassBy:
        szState = GetMultiLangString(idMainFmSt25); //"旁路";
        break;
      case COIL_ShutDownDelay:
        szState = GetMultiLangString(idMainFmSt26); //"待关机";
        break;
      case COIL_ShutDown:
        szState = GetMultiLangString(idMainFmSt27); //"待机";
        break;
      }

    if( 0 == BoardCtrl_GetInvertorPower() )
      GUI_DrawPicture( &picPowerCYDK22x36jpg, 
                        rtPanelsIcon_x1, rtPanelsIcon_y1(1) );
    else
      GUI_DrawPicture( &picPowerCYHI22x36jpg, 
                        rtPanelsIcon_x1, rtPanelsIcon_y1(1) );

//    if( GetRSTSrc(RRS_PORRST) )  // 冷启动
//      szState = "冷启动";

    if( nullptr == szState )
      szState = GetMultiLangString(idMainFmSt28); //"未知";
    
    GUI_SetColor( crFormBkg );
    GUI_FillRectEx( &Rect );
    
    GUI_SetColor( crPanelsLabelFont );
    GUI_DispStringInRect( szState, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER );
    }

  Rect.y0 = rtPanelsText_y1(2);
  Rect.y1 = rtPanelsText_y2(2);

  // ---------------- 电池容量
  iValue = (int)_GetCommonReg( REG_CM_BCHRG_Ibus );
  const TGUIPicture *pPicture = nullptr;
  if( -25 * RATIO_Current > iValue && 0 != Adapter_ON_State )
    {
    // 充电动画
    iValue = FpFormState->uPage1PanelsState[2];
    iValue++;
    if( 5 < iValue )
      iValue = 0;
    switch( iValue ) 
      {
      case 0:
        pPicture = &picBattery21x36Fbmp;
        break;
      case 1:
        pPicture = &picBattery21x36CHbmp;
        break;
      case 2:
        pPicture = &picBattery21x36Dbmp;
        break;
      case 3:
        pPicture = &picBattery21x36Cbmp;
        break;
      case 4:
        pPicture = &picBattery21x36Bbmp;
        break;
      case 5:
        pPicture = &picBattery21x36Abmp;
        break;
      }
    FpFormState->uPage1PanelsState[2] = iValue;
    }
  else
    {
    iValue = (int)_GetCommonReg( REG_CM_BCHRG_Level );
    iValue /= (RATIO_Percentage / 5);
    if( true == bAll ||
        iValue != FpFormState->uPage1PanelsState[2] )
      {
      switch( iValue )
        {
        case 0:
          pPicture = &picBattery21x36Fbmp;
          break;
        case 1:
          pPicture = &picBattery21x36Ebmp;
          break;
        case 2:
          pPicture = &picBattery21x36Dbmp;
          break;
        case 3:
          pPicture = &picBattery21x36Cbmp;
          break;
        case 4:
          pPicture = &picBattery21x36Bbmp;
          break;
        case 5:
          pPicture = &picBattery21x36Abmp;
          break;
        }
      FpFormState->uPage1PanelsState[2] = iValue;
      }
    }
  if( nullptr != pPicture )
    {
    //const TGUIPicture *pPicture = &picBattery21x36Bbmp;
    //GUI_SetColor( crFormBkg );
    //GUI_FillRect( rtPanelsIcon_x1, rtPanelsIcon_y1(2), 
    //              rtPanelsIcon_x2, rtPanelsIcon_y2(2) );
    GUI_DrawPicture( pPicture, 
                     rtPanelsIcon_x1,  rtPanelsIcon_y1(2) );
    //GUI_SetColor( crPanelsLabelFont );
    }

  iValue = (int)_GetCommonReg( REG_CM_BCHRG_Level );
  float fBatCap  = (float)(iValue) / RATIO_Percentage * 100.0f;
  uint32_t uStrId = 0;
  if( fBatCap >= 99.9999f )
    //sprintf( szStr, "电池\n%0.1f%%", fBatCap );
    uStrId = idMainFmSt31;
  else
    //sprintf( szStr, "电池\n%0.2f%%", fBatCap );
    uStrId = idMainFmSt32;
  sprintf( szStr, GetMultiLangString(uStrId), fBatCap );



  GUI_DispStringInRect( szStr, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER );

  // ---------------- 环境温度
  if( true == bAll )
    GUI_DrawPicture( &picThermometerCYAN19x36jpg, 
                      rtPanelsIcon_x1, rtPanelsIcon_y1(3) );

  Rect.y0 = rtPanelsText_y1(3);
  Rect.y1 = rtPanelsText_y2(3);
  
  iValue = (int)_GetCommonReg( REG_CM_BAT_TEMPERATRUE ); //REG_RTC_TEMPERATRUE );
  if( true == bAll ||
      iValue != FpFormState->uPage1PanelsState[3] )
    {
    float fTempe  = (float)(iValue) / RATIO_Temperature;
    //sprintf( szStr, "环境\n%0.1f%℃", fTempe );
    sprintf( szStr, GetMultiLangString(idMainFmSt41), fTempe );
    GUI_DispStringInRect( szStr, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER );
      
    FpFormState->uPage1PanelsState[3] = iValue;
    }
}
//-----------------------------------------------------------------------------
static void _drawPanels()
{
  
  // 
  GUI_AA_EnableHiRes();
  GUI_AA_SetFactor( HiAAF );  // AA因子

  GUI_SetColor( crPanelFrameHi );
  GUI_SetPenSize( 2 );
  GUI_SetPenShape( GUI_PS_SQUARE );
  GUI_SetLineStyle( GUI_LS_SOLID );

  for( int iIdx = 1; iIdx <= Num_Panels; iIdx++ )
    {
    int iy = rtPanelsArea_y1 + Height_Panel * iIdx;
    GUI_DrawGradientH( rtPanelsArea_x1, 
                       iy - 1,
                       rtPanelsArea_x2 - 10, 
                       iy,
                       crPanelFrameDark, crPanelFrameHi );

    GUI_AA_DrawArc ( HiAAF * (rtPanelsArea_x2 - 10), 
                     HiAAF * (iy - 10), 
                     HiAAF * 10, 
                     HiAAF * 10, 
                     -90, 0 );

    GUI_DrawGradientV( rtPanelsArea_x2 - 1, 
                       (iy - 40),
                       rtPanelsArea_x2,     
                       (iy - 10),
                       crPanelFrameDark, crPanelFrameHi );
    };

  GUI_AA_DisableHiRes();
   
  // 更新状态
  _updatePanels( true );
}
//-----------------------------------------------------------------------------
static void GUI_FillGradientCirile( int x0, int y0, int r, 
                                    GUI_COLOR Color0, GUI_COLOR Color1 )
{

  if( r < 2 )
    return ;
  
  int iColor0[3], iColor1[3];
  iColor0[0] = (Color0 & 0xFF);
  iColor0[1] = ((Color0 & 0xFF00) >> 8) & 0xFF;
  iColor0[2] = ((Color0 & 0xFF0000) >> 16) & 0xFF;
  iColor1[0] = (Color1 & 0xFF);
  iColor1[1] = ((Color1 & 0xFF00) >> 8) & 0xFF;
  iColor1[2] = ((Color1 & 0xFF0000) >> 16) & 0xFF;
  
  float fColorDelta[3];
  fColorDelta[0] = ((float)(iColor1[0] - iColor0[0])) / r;
  fColorDelta[1] = ((float)(iColor1[1] - iColor0[1])) / r;
  fColorDelta[2] = ((float)(iColor1[2] - iColor0[2])) / r;
  
  GUI_SetPenSize( HiAAF );
  for( int ir = r; ir > 0; ir -= HiAAF )
    {
    iColor0[0] = iColor1[0] - fColorDelta[0] * ir;
    iColor0[1] = iColor1[1] - fColorDelta[1] * ir;
    iColor0[2] = iColor1[2] - fColorDelta[2] * ir;
      
    GUI_COLOR color = iColor0[0] + (iColor0[1] << 8) + (iColor0[2] << 16);
    GUI_SetColor( color );
    GUI_AA_DrawArc( x0, y0, ir, ir, -180, 180 );
    }
}
//-----------------------------------------------------------------------------
static void _drawMeteor()
{

  if( 0 == FpFormState->meteor.hMeteorMem )
    FpFormState->meteor.hMeteorMem = GUI_MEMDEV_Create( 
                                             (meteor_cx - meteor_r) / HiAAF, 
                                             (meteor_cy - meteor_r) / HiAAF, 
                                             meteor_w / HiAAF, 
                                             meteor_h / HiAAF );

  GUI_MEMDEV_Clear(FpFormState->meteor.hMeteorMem);
  
  GUI_MEMDEV_Select(FpFormState->meteor.hMeteorMem);

  GUI_AA_EnableHiRes();
  GUI_AA_SetFactor( HiAAF );  // AA因子

  GUI_SetPenShape( GUI_PS_SQUARE );
  GUI_SetLineStyle( GUI_LS_SOLID );

  GUI_SetPenSize( 4 );
  GUI_SetColor( crMeteorFrameHi );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(2), metRad(2), meteor_as1, meteor_ae1 );

//  if( true == bAll )
    {
    GUI_SetPenSize( 2 );
    GUI_SetColor( crMeteorDialDark1 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(6), metRad(6), meteor_as1, meteor_ae1 );
    GUI_SetColor( crMeteorDialDark2 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(8), metRad(8), meteor_as1, meteor_ae1 );
    GUI_SetColor( crMeteorDialDark3 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(10), metRad(10), meteor_as1, meteor_ae1 );
    }

  GUI_SetPenSize( 10 );
  GUI_SetColor( crMeteorDial );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(15), metRad(15), meteor_as1, meteor_ae1 );

  GUI_SetPenSize( 2 );
  GUI_SetColor( crMeteorFrameHi );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(20), metRad(20), meteor_as1, meteor_ae1 );
  GUI_SetColor( crMeteorFrameDark );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(22), metRad(22), meteor_as1, meteor_ae1 );
  GUI_SetColor( crMeteorFrameHi );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(24), metRad(24), meteor_as1, meteor_ae1 );

  GUI_SetPenSize( 26 );
  GUI_SetColor( crMeteorNumDial );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(37), metRad(37), meteor_as1, meteor_ae1 );

  SetRSTSrc( RRS_GUITASK );
  osDelay(1);

  // 刻度
  GUI_SetPenSize ( 2 );
  GUI_SetFont    ( ftMeteorDial );
  GUI_SetTextMode( GUI_TEXTMODE_TRANS );
  for( int iIdx = 0; iIdx <= FpFormState->meteor.uSections; iIdx += 2 )
    {
//    int iDeg = meteor_IndDeg((float)iIdx / FpFormState->meteor.uSections) + 0.5;
    float fAngle = meteor_IndAng((float)iIdx / FpFormState->meteor.uSections), 
          fSin   = arm_sin_f32( fAngle ),
          fCos   = arm_cos_f32( fAngle );
//    if( meteor_as1 >= iDeg && meteor_ae1 < iDeg )
      {
      float x1 = meteor_cx + metRad(6)  * fCos,
            y1 = meteor_cy - metRad(6)  * fSin,
            x2 = meteor_cx + metRad(15) * fCos,
            y2 = meteor_cy - metRad(15) * fSin;
      GUI_SetColor( crMeteorDialLine );
      GUI_AA_DrawLine( x1, y1, x2, y2 );
      }

    if( 12 != iIdx )
      {
      float x1 = (meteor_cx + metRad(30) * fCos) / HiAAF,
            y1 = (meteor_cy - metRad(30) * fSin) / HiAAF - 3;
      if( iIdx >= FpFormState->meteor.uSections / 2 )
        x1 -= 12;
      
      char cBuf[4];
      sprintf( cBuf, "%d", iIdx * (FpFormState->meteor.uUpLimit / 
                                   FpFormState->meteor.uSections) );
      GUI_SetColor( crMeteorTextDial );
      GUI_DispStringAt( cBuf, x1, y1 );
      }
    }

  SetRSTSrc( RRS_GUITASK );
  osDelay(1);

  // 内表盘
  GUI_SetPenSize( 1 );
  GUI_SetColor( crMeteorShaftDark );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(48), metRad(48), -180, 180 );

  GUI_SetPenSize( 2 );
  GUI_SetColor( crMeteorFrameHi );
  GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(50), metRad(50), -180, 180 );

  // 表心转轴
//  if( true == bAll )
    {
    GUI_FillGradientCirile( meteor_cx, meteor_cy, metRad(52), 
                            crFormBkg, GUI_BLUE );

    GUI_SetFont ( ftMeteorShaftText );
    GUI_SetColor( crMeteorShaftText );
    float x1 = meteor_cx / HiAAF - 8,
          y1 = meteor_cy / HiAAF - 16;
    GUI_DispStringAt( "V", x1, y1 );

    SetRSTSrc( RRS_GUITASK );
    osDelay(1);

    // 表心的高光效果
    GUI_SetPenSize( 2 );
    GUI_SetColor( GUI_WHITE );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(58), metRad(58), 121, 169 );
    GUI_SetColor( GUI_GRAY_D0 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(60), metRad(60), 122, 168 );
    GUI_SetColor( GUI_GRAY_C0 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(62), metRad(62), 123, 167 );
    GUI_SetColor( GUI_GRAY_9A );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(64), metRad(64), 123, 167 );
    GUI_SetColor( GUI_GRAY_7C );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(66), metRad(66), 124, 166 );
    GUI_SetColor( GUI_GRAY_50 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(68), metRad(68), 124, 166 );

    SetRSTSrc( RRS_GUITASK );
    osDelay(1);

    GUI_SetPenSize( 1 );
    GUI_SetColor( GUI_GRAY_50 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55), metRad(55),  60, 120 );
    GUI_SetColor( GUI_GRAY_7C );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55), metRad(55),  70, 110 );
    GUI_SetPenSize( 2 );
    GUI_SetColor( GUI_GRAY_9A );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55) - 1, metRad(55) - 1, 80, 100 );
    GUI_SetPenSize( 1 );
    GUI_SetColor( GUI_GRAY_C0 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55) - 1, metRad(55) - 1, 82,  98 );
    GUI_SetColor( GUI_GRAY_D0 );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55) - 1, metRad(55) - 1, 84,  96 );
    GUI_SetColor( GUI_WHITE );
    GUI_AA_DrawArc( meteor_cx, meteor_cy, metRad(55) - 2, metRad(55) - 2, 87,  93 );
    }

  GUI_AA_DisableHiRes();

  GUI_MEMDEV_Select( 0 );
}
//-----------------------------------------------------------------------------
static void _updateIndicator( TCommonReg uValue)
{

  GUI_AA_EnableHiRes();
  GUI_AA_SetFactor( HiAAF );  // AAl因子

  GUI_SetPenSize( 1 );
  GUI_SetPenShape( GUI_PS_SQUARE );
  GUI_SetLineStyle( GUI_LS_SOLID );
  // Draw Indicator
//  if( false == bEarse )
    {
    float fValue = (float)(uValue) / RATIO_Voltage;
    int fDeg = meteor_IndDeg(fValue / FpFormState->meteor.uUpLimit) + 0.5f;
    if( meteor_as1 + 3 < fDeg && meteor_ae1 - 3 > fDeg )
      {
      float fAngle = meteor_IndAng(fValue / FpFormState->meteor.uUpLimit), 
            fSin   = arm_sin_f32( fAngle ),
            fCos   = arm_cos_f32( fAngle );

      // 旋转指针
      GUI_POINT ptInd[NUM_Indicators];
      for( int iIdx = 0; iIdx < NUM_Indicators; iIdx++ )
        {
        int x = ptIndicator[iIdx].x * HiAAF,
            y = ptIndicator[iIdx].y * HiAAF;
        ptInd[iIdx].x =   x * fCos - y * fSin  + 0.5f;
        ptInd[iIdx].y = -(x * fSin + y * fCos) - 0.5f;
        }
      GUI_SetColor( crMeteorIndicator );
      GUI_AA_FillPolygon( ptInd, NUM_Indicators, meteor_cx, meteor_cy );
      
      // 用内表盘框颜色覆盖指针根部
      GUI_SetPenSize( 2 );
      GUI_SetColor( crMeteorFrameHi );
      GUI_AA_DrawArc( meteor_cx,  meteor_cy, 
                      metRad(50), metRad(50), 
                      fDeg - 3, fDeg + 3);
      }
    }

  GUI_AA_DisableHiRes();
}
//-----------------------------------------------------------------------------
static void _updateMeteor( bool bAll, TCommonReg uValue)
{
  
  int iAndleBeg, iAndleEnd;
  
  // 
  if( true == bAll )
    {
    iAndleBeg = meteor_as1;
    iAndleEnd = meteor_ae1;
      
    _drawMeteor();

    osDelay(10);
    }

  // 将内存设备中画好的表盘显示到屏幕
  GUI_MEMDEV_CopyToLCD(FpFormState->meteor.hMeteorMem);
    
  osDelay(10);

  // 画表指针
  _updateIndicator( uValue );
}
//-----------------------------------------------------------------------------
static void _updateVoltageDisp( bool bAll )
{

  // update Voltage Value
  GUI_RECT Rect = {rtValue_x1, rtValue_y1, rtValue_x2, rtValue_y2};

  char cStr[8];
  TCommonReg uValue = _GetCommonReg(REG_CM_Uin);
  if( 100 * RATIO_Voltage <= uValue && 
      (true == bAll || uValue != FpFormState->uVoltageValue) )
    {
    sprintf( cStr, "%0.2f", (float)(uValue) / RATIO_Voltage );
    
    if( 0 < FpFormState->uVoltageTick )
      {
      GUI_SetColor( crFormBkg );
      GUI_FillRectEx( &Rect );
        
      FpFormState->uVoltageTick = 0;
      }
      
    GUI_SetColor( crVoltageFont );
    GUI_SetFont ( ftVoltageFont );
   
    GUI_SetTextMode( GUI_TEXTMODE_NORMAL );
    GUI_DispStringInRect( cStr, &Rect, GUI_TA_CENTER | GUI_TA_VCENTER );
    }
  else if( 100 * RATIO_Voltage > uValue )
    {
    FpFormState->uVoltageTick++;

    if( 1 == (FpFormState->uVoltageTick & 1) )
      {
      GUI_SetColor( crVoltageLost );
      GUI_FillRectEx( &Rect );

      GUI_SetColor( crVoltageFont );
      }
    else
      {
      GUI_SetColor( crFormBkg );
      GUI_FillRectEx( &Rect );

      GUI_SetColor( crVoltageLost );
      }

    GUI_SetFont ( ftVoltageLost );
    GUI_SetTextMode( GUI_TEXTMODE_TRANS );
    GUI_DispStringInRect( GetMultiLangString(idMainFmSt51), //"失压", 
                          &Rect, GUI_TA_CENTER | GUI_TA_VCENTER );
    }

  FpFormState->uVoltageValue = uValue;

//  Rect.y0 -= 20;
//  Rect.y1  = Rect.y0  + 18;
//  GUI_SetColor( GUI_GRAY_9A );
//  GUI_SetFont ( GUI_FONT_16LTH_CHN );
//  GUI_DispStringInRect( "交流电压", &Rect, GUI_TA_CENTER | GUI_TA_VCENTER );

  // Update Meteor
  if( true == bAll ||
      (int)uValue > (int)FpFormState->meteor.uLastVoltage + RATIO_Voltage ||
      (int)uValue < (int)FpFormState->meteor.uLastVoltage - RATIO_Voltage )
    {
    uint32_t uUpLimit;
    if( uValue > 260 * RATIO_Voltage )
      uUpLimit = 480;
    else
      uUpLimit = 260;
    
    if( uUpLimit != FpFormState->meteor.uUpLimit )
      {
      FpFormState->meteor.uUpLimit  = uUpLimit;
        
      if( 260 == uUpLimit )
        FpFormState->meteor.uSections = 26;
      else
        FpFormState->meteor.uSections = 24;
      
      bAll = true;
      }
      
    _updateMeteor( bAll, uValue );

    FpFormState->meteor.uLastVoltage = uValue;
    }
}
//-----------------------------------------------------------------------------
static void _updatePage1( bool bAll )
{

  // update Caption
  _updateCaption1( bAll );

  // update Info-panels
  _updatePanels( bAll );
  
  // udpate voltage display
  _updateVoltageDisp( bAll );
}
//-----------------------------------------------------------------------------
static void _showPage1()
{
  
  // Draw Caption
  _updateCaption1( true );
  
  osDelay(10);
  
  // Draw Panels
  _drawPanels();

  osDelay(10);
  
  // Draw Meteor
  TCommonReg uValue = _GetCommonReg(REG_CM_Uin);
  _updateMeteor( true, uValue );

  osDelay(10);
  
  FpFormState->meteor.uLastVoltage = uValue;

  // 
  GUI_RECT Rect = { rtValue_x1, 
                    rtValue_y1 - 32, 
                    rtValue_x2, 
                    rtValue_y1 - WIDTH_EDGE };
  GUI_SetColor( GUI_GRAY_9A );
  GUI_SetFont ( GUI_FONT_16LTH_CHN );
  GUI_DispStringInRect( GetMultiLangString(idMainFmSt52), //"交流电压", 
                        &Rect, GUI_TA_CENTER | GUI_TA_VCENTER );
}
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//// Page 2
////-----------------------------------------------------------------------------
//static void _showPage2()
//{

//  GUI_SetColor( crFrameHigh );
//  GUI_RECT Rect = { CapLabel_x1, CapLabel_y1, CapLabel_x2, CapLabel_y2 };
//  GUI_DrawRectEx( &Rect );

//  GUI_SetColor( crCaptionFont );
//  GUI_SetFont( GUI_FONT_24LTH_CHN );
//  GUI_SetTextMode( GUI_TEXTMODE_TRANS );
//  GUI_DispStringInRect( "通信统计", &Rect, GUI_TA_CENTER | GUI_TA_VCENTER );
//  
//  // Browser
//  GUI_SetColor( crFrameHigh );
//  GUI_DrawRect( rtBrowser_x1,  rtBrowser_y1, rtBrowser_x2,  rtBrowser_y2 );
//}
////-----------------------------------------------------------------------------
//static void _updatePage2( bool bAll )
//{
//}
////-----------------------------------------------------------------------------
//static void _UpdateClock()
//{
//  
//  osMutexWait( FGUIState.mutexGUI, osWaitForever );

//  GUI_SetBkColor( crFormBkg );
//  GUI_SetColor( crClockFont );
//  GUI_SetFont( GUI_FONT_6X8_ASCII );
//  GUI_SetTextMode( GUI_TEXTMODE_NORMAL );

//  osMutexRelease( FGUIState.mutexGUI ); 
//}
//-----------------------------------------------------------------------------
static void _updateForm()
{

//  _UpdateClock();
  
  switch( FpFormState->uPageIdx )
    {
    case 0:
      {
      _updatePage1( false );
      break;
      }

//    case 1:
//      {
//      _updatePage2( false );
//      break;
//      }
    }
}
//-----------------------------------------------------------------------------
static void _showForm()
{

  GUI_SetBkColor( crFormBkg );
  GUI_Clear();
  
  switch( FpFormState->uPageIdx )
    {
    case 0:
      {
      _showPage1();
        
      _updatePage1( true );

      break;
      }
      
//    case 1:
//      {
//      _showPage2();
//        
//      _updatePage2( true );

//      break;
//      }
    }
}
//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
static void _Init(void const * argument)
{
  
  if( nullptr == FpFormState )
    FpFormState = (TMainFormState*)RAM_Malloc( sizeof(TMainFormState) );
  
  memset( FpFormState, 0, sizeof(TMainFormState) );
  
  FpFormState->uNextTick = GUI_GetTime() + INTEVAL_UPDATE;
  
  FpFormState->uMsgIdx  = 0;
  
  FpFormState->meteor.uUpLimit  = 260;
  FpFormState->meteor.uSections = 26;
}
//-----------------------------------------------------------------------------
static void _Show(void const * argument)
{

  _showForm();
}
//-----------------------------------------------------------------------------
static void _Close(void const * argument)
{
  
  if( 0 != FpFormState->meteor.hMeteorMem )
    GUI_MEMDEV_Delete( FpFormState->meteor.hMeteorMem );
  
  RAM_Free( FpFormState );
  FpFormState = nullptr;
}
//-----------------------------------------------------------------------------
static void _OnTick(uint32_t uTick)
{

  if( FpFormState->uNextTick <= uTick )
    {
    FpFormState->uNextTick = GUI_GetTime() + INTEVAL_UPDATE;
      
    _updateForm();
    }
}
//-----------------------------------------------------------------------------
static void _OnKeyDown(uint16_t uwKey)
{

  switch(uwKey)
    {
    case KEY_RIGHT:
      {
#if PAGES_IN_FORM > 1
      if( PAGES_IN_FORM - 1 > FpFormState->uPageIdx )
        {
        FpFormState->uPageIdx++;
          
        _showForm();
        }
#else
      GUIFormSwitch( WID_MenuForm, 0 );
#endif
      break;
      }
      
    case KEY_LEFT:
      {
#if PAGES_IN_FORM > 1
      if( 0 < FpFormState->uPageIdx )
        {
        FpFormState->uPageIdx--;
          
        _showForm();
        }
#endif
      break;
      }
      
    case KEY_UP:
      {
      GUIFormSwitch( WID_DevInfoForm, 0 );

      break;
      }

    case KEY_DOWN:
      {
      GUIFormSwitch( WID_SplashForm, 0 );

      break;
      }
      
    case KEY_ENTER:
      GUIFormSwitch( WID_MenuForm, 0 );
      break;
    
    case KEY_ESCAPE:
      GUIFormSwitch( WID_SplashForm, 0 );
      break;
    }
}
//-----------------------------------------------------------------------------
static void _OnMessage(GM_MESSAGE* pMsg)
{
  
  if( 0 == pMsg )
    return ;
  
  switch( pMsg->MsgId )
    {
    case GM_TIMER_TICK:
      { 
      _OnTick( pMsg->Data.v );
        
      break;
      }
      
    case GM_KEYDOWN:
      {
      if( pMsg->Param )
        {
        _OnKeyDown( pMsg->Param );
          
        pMsg->MsgId = 0;
        }
        
      break;
      }
      
//    case GM_TIMERECV:
//      {
//      if( FGUIState.pCurForm == &FMainForm )
//        _UpdateClock();
//      
//      break;
//      }
      
    case GM_DATARECV:
      {
      
      break;
      }
    }
}
//=============================================================================
// 窗体句柄
//-----------------------------------------------------------------------------
const GWinForm FMainForm = 
{
  _Init,
  _Show,
  _Close,
  _OnMessage
};
//-----------------------------------------------------------------------------
