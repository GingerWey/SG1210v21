//-----------------------------------------------------------------------------
/*
   emWin��������Դ
   
   ����ƽ
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
// ȫ�ֱ���
//---------------------------------------------------------------------------
// 8x16 ASCII
extern const GUI_FONT GUI_FontASCII16;
#define GUI_FONT_ASCII16 (&GUI_FontASCII16)

// 12x24 ASCII
extern const GUI_FONT GUI_FontASCII24;
#define GUI_FONT_ASCII24 (&GUI_FontASCII24)

// 16x16 CHN
extern const GUI_FONT GUI_CHNFont16LTH;
extern const GUI_FONT GUI_CHNFont16LTHx2;
#define GUI_FONT_16LTH_CHN   (&GUI_CHNFont16LTH)

// 24x24 CHN
extern const  GUI_FONT GUI_CHSLTHFont24;
extern const  GUI_FONT GUI_CHSLTHFont24x2;
#define GUI_FONT_24LTH_CHN   (&GUI_CHSLTHFont24)

// 17X24 ASCII  CAPTAIN
extern const GUI_FONT GUI_Font_ASCCAP24B;
#define GUI_FONT_DIGCAP24B   (&GUI_Font_ASCCAP24B)

#ifdef __vmSIMULATOR__
extern const GUI_FONT GUI_FontConsolas16B;
#define GUI_FONT_ASCII16B    (&GUI_FontConsolas16B)
#else
// 16 ASCII AA4
extern const GUI_FONT GUI_Font_AA4_16_L;
#define GUI_FONT_AA4_ASCII16L   (&GUI_Font_AA4_16_L)

extern const GUI_FONT GUI_Font_AA4_16_B;
#define GUI_FONT_ASCII16B    (&GUI_Font_AA4_16_B)
#endif
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
#endif //GUI_FONTSGRES_H
