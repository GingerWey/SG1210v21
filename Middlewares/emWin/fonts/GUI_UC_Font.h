//-----------------------------------------------------------------------------
/*
 File        : GUI_UC_EncodeNoneX.h
 Version     : V1.10
 By          : 银网科技

 For         : emWin
 Mode        : Thumb2
 Description :扩展非ACSII的Uniocde字符显示方法
        
 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#ifndef __GUI_UC_FONTX_H
#define __GUI_UC_FONTX_H

#include "GUI_Type.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"  {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
//=============================================================================
// 字模资源表
//---------------------------------------------------------------------------
// 字模数据描述项
typedef struct _GUI_FontItem
{
        unsigned char   Char[3];       // 字符
  const unsigned char  *DotMix;        // 点阵
} TGUIFontItem;
//-----------------------------------------------------------------------------
typedef struct _GUI_Font_Type
{
  unsigned char       Width;          // 宽
  unsigned char       Height;         // 高
  unsigned char       BaseLine;       // 基线位置
  unsigned char       BytesPerLine;   // 每行字节数
} TGUIFontType;
//-----------------------------------------------------------------------------
// 字模数据描述
typedef struct _GUI_FontList
{
  TGUIFontType          Type;   // 字体描述
  unsigned short        Count;  // 字符数量
  const TGUIFontItem   *Items;  // 字模
} TGUIFontList;
//=============================================================================
// 扩展UC字体显示方法
// 本宏用于加在GUI_FONT实例中，简化UC字体定义
//---------------------------------------------------------------------------
#define GUI_UC_FONTTYPE_PROP       \
  GUIPROP_UC_DispChar,             \
  GUIPROP_UC_GetCharDistX,         \
  GUIPROP_UC_GetFontInfo,          \
  GUIPROP_UC_IsInFont,             \
  GUIPROP_UC_GetCharInfo,          \
  (tGUI_ENC_APIList*)0    
//=============================================================================
// 公用方法
//-----------------------------------------------------------------------------
DECLARE_FONT(PROP_UC);
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
#endif  // __GUI_UC_FONTX_H
