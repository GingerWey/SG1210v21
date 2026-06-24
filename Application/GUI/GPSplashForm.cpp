//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 File        : GPSplashForm.cpp
 Version     : V1.10
 By          : Silver Grid Technology

 Description : Splash form implementation.
               Shows logo and CSG image on startup, auto-transitions to main form.

 Date        : 2023.12.05
*/
//-----------------------------------------------------------------------------//-----------------------------------------------------------------------------
#include "GPSplashForm.h"

#include "GUI.h"
#include "WM.h"
#include "GUI_Private.h"
#include "GWinTypes.h"
#include "FontCHS24LTH.h"
#include "FontDigital44B.h"

#include "GForm.h"
#include "GUICntr.h" // MCU: FGUIState, osWaitForever
#include "GUIConf.h"
#include "GUIMisc.h"
#include "GUIMessage.h"

#include "GUIPicture.h"
#include "PictureRes.h"
#include "Graphics/x128.h"
#include "Strings/TextStrs.h"

#include <stdlib.h>
#include <string.h>
#include "GFormRegistrar.h"

//=============================================================================
// 
//-----------------------------------------------------------------------------
#define  SPLASH_BK_COLOR   GUI_MAKE_COLOR(0x00C0FFB6)
//=============================================================================
// Data
//-----------------------------------------------------------------------------
typedef struct tagFormState
{
  uint32_t uNextTick;
} TFormState;
//=============================================================================
// Data
//-----------------------------------------------------------------------------
static TFormState m_FormState;
//=============================================================================
// 
//-----------------------------------------------------------------------------
static void _FlushForm()
{
  
  GUI_SetBkColor( SPLASH_BK_COLOR );
  GUI_Clear();
//  GUI_DrawPicture( &picSplashBkg320x240jpg, 0, 0 );
}
//-----------------------------------------------------------------------------
static void _UpdateGraph()
{
#ifndef __vmSIMULATOR__ 
  osMutexWait( FGUIState.mutexGUI, osWaitForever );
#endif

//  GUI_DrawBitmap( &bmasd03s, 0, 0 );
  GUI_DrawPicture( &picSGLogo200x43C565bmp, 60, 50 );

  GUI_SetColor( GUI_DARKGREEN );
  GUI_SetTextMode( GUI_TEXTMODE_TRANS );

  GUI_SetFont(GUI_FONT_24LTH_CHN);
  GUI_DispStringAt("SG1210 v3.1", 100, 110);

  GUI_SetFont( GUI_FONT_24LTH_CHN );
  const char* pStr = GetMultiLangString(idDevFuncSUTC);
  GUI_DispStringAt( pStr, 40, 140 );

  GUI_SetColor( GUI_ORANGE );
  GUI_SetFont( &GUI_Font8x12_ASCII );
  GUI_DispStringAt( "Powered by SilverGrid", 75, 170 );

#ifndef __vmSIMULATOR__
  osMutexRelease(FGUIState.mutexGUI);
#endif
}
//=============================================================================
// Global methods
//-----------------------------------------------------------------------------
static void _Init(const void* argument)
{
  
  memset( &m_FormState, 0, sizeof(m_FormState) );
}
//-----------------------------------------------------------------------------
static void _Show(const void* argument)
{
  
  _FlushForm();
  GUI_DrawPicture(&picx128csg, 100, 100);
  
  _UpdateGraph();
  
  m_FormState.uNextTick = GUI_GetTime() + 1000;
}
//-----------------------------------------------------------------------------
static void _Close(const void* argument)
{
}
//-----------------------------------------------------------------------------
static void _OnTick(uint32_t uTick)
{

#if (DEBUG_COMPAR == 0)
  if( uTick > m_FormState.uNextTick ) {
        gform::PopForm();
        gform::PushForm(WID_MainForm, nullptr);
      }
        
#endif   
}
//-----------------------------------------------------------------------------
static void _OnKeyUp(uint16_t uwKey)
{

 if ((KEY_LEFT | KEY_RIGHT) == (uwKey & (KEY_LEFT | KEY_RIGHT))) {
        gform::PushForm(WID_DevInfoForm, nullptr); // SwitchForm
    } else if (uwKey == KEY_ENTER)
        gform::PushForm(WID_MainForm, nullptr);
}
//-----------------------------------------------------------------------------
static void _OnMessage( GM_MESSAGE* pMsg)
{
  
  if( nullptr == pMsg )
    return ;
  
  switch( pMsg->MsgId )
    {
    case GM_TIMER_TICK:
      { 
      _OnTick( pMsg->Data.v );
        
      break;
      }
      
    case GM_KEYUP:
      {
      if( pMsg->Param )
        {
        _OnKeyUp( pMsg->Param );
          
        pMsg->MsgId = 0;
        }
        
      break;
      }
    }
}
//=============================================================================
// Form
//-----------------------------------------------------------------------------
const GWinForm FSplashForm = 
{
  _Init,
  _Show,
  _Close,
  _OnMessage
};

// Auto-register with new GForm system
static const gform::FormRegistrar kRegSplash(WID_SplashForm, &FSplashForm, "Splash");
//-----------------------------------------------------------------------------
