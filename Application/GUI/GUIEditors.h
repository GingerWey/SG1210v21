//-----------------------------------------------------------------------------
/*
 File        : GUIEditors.h
 Version     : V1.10
 By          : 银网科技
 Description :定义各种编辑器
        
 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#ifndef GUI_EDITORS_H
#define GUI_EDITORS_H

#include <GUI.h>
#include <cstdint>
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// 编辑器类型
#define EDT_NUMBER      0x01
#define EDT_LIST        0x02

#define EDT_BUTTON      0xAA

//=============================================================================
// 全局数据结构
//-----------------------------------------------------------------------------
typedef struct tagLabelObj
{

  GUI_RECT        rect;
  
  const GUI_FONT *pFont;
  uint32_t        crText;
  uint32_t        uAlign;

  const uint32_t  uCaption;
} TLabelObj;
//-----------------------------------------------------------------------------
// 编辑器静态数据
typedef struct tagEditorObj
{
  
  // Label
  TLabelObj       lbCaption,
                  lbDim;
  
  // Editor
  GUI_RECT        rect;
  
  const GUI_FONT *pFont;
  uint32_t        uAlign;
  
  struct tagEditorColor {
    GUI_COLOR     crBkg;
    GUI_COLOR     crBorder;
    GUI_COLOR     crInfo;
    } Normal, Focus;
  
  uint32_t        uType;
  uint32_t        uMin;
  uint32_t        uMax;
  
  const void      *pList;
} TEditorObj;
//=============================================================================
// 全局对象
//-----------------------------------------------------------------------------
class TEditor
{
protected:
  const TEditorObj*  m_pEditor;
  uint32_t           m_uValue, m_uOrgValue;

  bool               m_bFocused = false;
  bool               m_bEditing = false;

  uint32_t           m_uLastTick = 0;
  bool               m_bTwinkle  = false;

public:
  TEditor(const TEditorObj* pEditor, uint32_t uValue = 0) 
      { 
        m_pEditor  = pEditor; 
        m_uValue   = uValue; 
      }
      
  // 设置编辑值
  void setValue( uint32_t uValue, bool bDraw = false);
  uint32_t getValue() { return m_uValue; }
  
  // 置焦点状态
  void setFocused( bool bFocused );
  
  // 置编辑状态
  void setEditing( bool bValue );
  bool getEditing(void) { return m_bEditing; }
 
  // 重新绘制
  // bALl=true时，绘标题
  void repint(bool bAll=false);

  // 响应按键消息
  void onKeyDown( uint32_t uKey, uint32_t uPress = 0);
  
  // 定时消息
  void onTick( uint32_t uTick );

protected:
  // 绘制Label
  void drawLabel(void);

  // 绘制编辑器
  void drawEditor(bool bAll=true);

  // 绘制按钮
  void drawButton(void);

  // 检查数据保法性
  uint32_t checkValue( uint32_t uValue );
};
//=============================================================================
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif // GUI_EDITORS_H
