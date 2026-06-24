//-----------------------------------------------------------------------------
/*
   emWin的字体资源
   
   卫荣平
   2026.6.6   
*/
//-----------------------------------------------------------------------------
#ifndef GUI_FONTSGRES_H
#define GUI_FONTSGRES_H

//-----------------------------------------------------------------------------
#include "GUI_Type.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// 全局变量
//---------------------------------------------------------------------------
// 8x16 ASCII
extern const GUI_FONT GUI_FontASCII16;
#define GUI_FONT_ASCII16 (&GUI_FontASCII16)

// 12x24 ASCII
extern const GUI_FONT GUI_FontASCII24;
#define GUI_FONT_ASCII24 (&GUI_FontASCII24)

// 16x16 CHN
extern GUI_CONST_STORAGE GUI_FONT GUI_CHNFont16LTH;
extern GUI_CONST_STORAGE GUI_FONT GUI_CHNFont16LTHx2;
#define GUI_FONT_16LTH_CHN   (&GUI_CHNFont16LTH)

// 24x24 CHN
extern GUI_CONST_STORAGE  GUI_FONT GUI_CHSLTHFont24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_CHSLTHFont24x2;
#define GUI_FONT_24LTH_CHN   (&GUI_CHSLTHFont24)

// 23x32 ASCII AA2
extern const GUI_FONT GUI_FontDigital32B;
#define GUI_FONT_AA2_DIGITAL32B (&GUI_FontDigital32B)

// 34x40 Digital AA2
extern const GUI_FONT GUI_FontDigital40;
#define GUI_FONT_AA2_DIGITAL40 (&GUI_FontDigital40)

// 32x44 Digital-Bold  AA2
extern const GUI_FONT GUI_FontDigital44B;
#define GUI_FONT_AA2_DIGITAL44B (&GUI_FontDigital44B)

// 16 ASCII AA4
extern const GUI_FONT GUI_Font_AA4_16_L;
#define GUI_FONT_AA4_ASCII16L   (&GUI_Font_AA4_16_L)
extern const GUI_FONT GUI_Font_AA4_16_B;
#define GUI_FONT_AA4_ASCII16B   (&GUI_Font_AA4_16_B)
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
#endif //GUI_FONTSGRES_H
