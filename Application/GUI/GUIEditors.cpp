//-----------------------------------------------------------------------------
/*
 File        : GUIEditors.cpp
 Version     : V1.10
 By          : 银网科技
 Description : 实现各种编辑器

 Date        : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "GUIEditors.h"

#include "GUI.h"
#include "GUICntr.h"
#include "GUIConf.h"
#include "Strings/TextStrs.h"

#include <stdio.h>
#include <string.h>
//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------

//=============================================================================
// 本地数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// 本地数据
//-----------------------------------------------------------------------------

//=============================================================================
// 本地方法
//-----------------------------------------------------------------------------

//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------

//=============================================================================
// 本地对象
//-----------------------------------------------------------------------------
// 检查数据保法性
uint32_t TEditor::checkValue( uint32_t uValue )
{

  if( m_pEditor->uMin > uValue )
    uValue = m_pEditor->uMin;

  if( m_pEditor->uMax < uValue )
    uValue = m_pEditor->uMax;
  
  return uValue;
}
//-----------------------------------------------------------------------------
void TEditor::setValue( uint32_t uValue, bool bDraw )
{
  
  if( uValue != m_uValue )
    {
    m_uValue = checkValue( uValue );
    
    if( true == bDraw )
      drawEditor();
    }
}
//-----------------------------------------------------------------------------
void TEditor::setFocused( bool bFocused )
{ 

  if( m_bFocused != bFocused )
    {
    m_bFocused = bFocused; 

    if( false == m_bFocused )
      {
      m_uValue = checkValue( m_uValue );
      }

    repint();
    }
}
//-----------------------------------------------------------------------------
void TEditor::setEditing( bool bEditing )
{
  
  if( EDT_BUTTON == m_pEditor->uType )
    m_bEditing = false;
  else if( bEditing != m_bEditing )
    {
    m_bEditing = bEditing;

    if( false == bEditing )
      {
      m_uValue = checkValue( m_uValue );
      }
    else
      {
      m_uOrgValue = m_uValue;
      
      m_uLastTick = 0;
      m_bTwinkle  = true;
      }

    drawEditor();
    }
}
//-----------------------------------------------------------------------------
void TEditor::repint(bool bAll)
{
  
  // 绘制标题
  if( true == bAll )
    drawLabel();
  
  if( EDT_BUTTON == m_pEditor->uType )
    drawButton();
  else
    // 绘制编辑器
    drawEditor();
}
//-----------------------------------------------------------------------------
// 绘制Label
void TEditor::drawLabel(void)
{

  if( EDT_BUTTON == m_pEditor->uType )
    return;

  const char* pStr = GetMultiLangString( m_pEditor->lbCaption.uCaption );
  if( nullptr != pStr )
    {
    GUI_SetFont ( m_pEditor->lbCaption.pFont );
    GUI_SetColor( m_pEditor->lbCaption.crText );
    GUI_SetTextMode ( GUI_TEXTMODE_TRANS );
      
    GUI_RECT rect = m_pEditor->lbCaption.rect;
    GUI_DispStringInRect( pStr, 
                         &rect, 
                          m_pEditor->lbCaption.uAlign );
    }
    
  pStr = GetMultiLangString( m_pEditor->lbDim.uCaption );
  if( nullptr != pStr )
    {
    GUI_SetFont ( m_pEditor->lbDim.pFont );
    GUI_SetColor( m_pEditor->lbDim.crText );
    GUI_SetTextMode ( GUI_TEXTMODE_TRANS );
    
    GUI_RECT rect = m_pEditor->lbDim.rect;
    GUI_DispStringInRect( pStr, 
                         &rect, 
                          m_pEditor->lbDim.uAlign );
    }
}
//-----------------------------------------------------------------------------
// 绘制编辑器
void TEditor::drawEditor(bool bAll)
{

  GUI_COLOR color;
  if( true == bAll )
    {
    // Background
    if( true == m_bFocused )
      color = m_pEditor->Focus.crBkg;
    else
      color = m_pEditor->Normal.crBkg;

    GUI_SetColor( color );
    GUI_FillRectEx( &(m_pEditor->rect) );

    // Frame
    if( true == m_bFocused )
      color = m_pEditor->Focus.crBorder;
    else
      color = m_pEditor->Normal.crBorder;
    
    GUI_SetColor( color );
    GUI_SetPenSize( 1 );
    GUI_SetLineStyle( GUI_LS_SOLID );
    GUI_DrawRectEx( &(m_pEditor->rect) );
    }

  // Caption
  char szBuf[12];
  const char* pBuf;
  switch( m_pEditor->uType )
    {
    case EDT_NUMBER:
      {
      sprintf( szBuf, "%u", m_uValue );
      pBuf = szBuf;
      break;
      }

    case EDT_LIST:
      {
      if( nullptr != m_pEditor->pList )
        {
        const uint32_t* puList = (const uint32_t*)m_pEditor->pList;
        pBuf = GetMultiLangString(puList[m_uValue]);
        }
      else
        pBuf = nullptr;
      
      break;
      }

    case EDT_BUTTON:
      {
      pBuf = GetMultiLangString( m_pEditor->lbCaption.uCaption );
      break;
      }

    default:
      {
      pBuf = nullptr;
      break;
      }
    }

  if( nullptr != pBuf )
    {
    GUI_SetFont( m_pEditor->pFont );
      
    if( true == bAll )
      GUI_SetTextMode ( GUI_TEXTMODE_TRANS );
    else
      GUI_SetTextMode ( GUI_TEXTMODE_NORMAL );

    if( true == m_bFocused )
      {
      if( true == m_bEditing )
        {
        if( true == m_bTwinkle )
          color = m_pEditor->Focus.crInfo;
        else
          color = m_pEditor->Normal.crInfo;
        
        GUI_SetBkColor( m_pEditor->Focus.crBkg );
        }
      else
        color = m_pEditor->Focus.crInfo;
      }
    else
      color = m_pEditor->Normal.crInfo;
    
    GUI_SetColor( color );
      
    GUI_RECT rect;
    rect.x0 = m_pEditor->rect.x0 + 8;
    rect.y0 = m_pEditor->rect.y0 - 4;
    rect.x1 = m_pEditor->rect.x1 - 8;
    rect.y1 = m_pEditor->rect.y1 + 4;

    GUI_DispStringInRect( pBuf, &rect, m_pEditor->uAlign );
    }
}
//-----------------------------------------------------------------------------
// 绘制编辑器
void TEditor::drawButton()
{

  GUI_COLOR color;
  if( true == m_bFocused )
    {
    color = m_pEditor->Focus.crBkg;
    }
  else
    {
    color = m_pEditor->Normal.crBkg;
    }
  GUI_SetColor( color );
  GUI_FillRectEx( &(m_pEditor->rect) );

  if( true == m_bFocused )
    {
    color = m_pEditor->Focus.crBorder;
    }
  else
    {
    color = m_pEditor->Normal.crBorder;
    }
  GUI_SetColor( color );
  GUI_DrawRoundedFrameEx( &(m_pEditor->rect), 3, 1 );

  const char* pBuf = GetMultiLangString( m_pEditor->lbCaption.uCaption );
  if( nullptr != pBuf )
    {
    GUI_SetFont( m_pEditor->pFont );
      
    GUI_SetTextMode ( GUI_TEXTMODE_TRANS );

    if( true == m_bFocused )
      {
      color = m_pEditor->Focus.crInfo;
      }
    else
      {
      color = m_pEditor->Normal.crInfo;
      }
    GUI_SetColor( color );
    
    GUI_RECT rect = m_pEditor->rect;
    GUI_DispStringInRect( pBuf, &rect, m_pEditor->uAlign );
    }
}
//-----------------------------------------------------------------------------
void TEditor::onKeyDown( uint32_t uKey, uint32_t uPress)
{

  switch( uKey )
    {
    case KEY_ADD:
    case KEY_UP:
      {
      uint32_t uValue = m_uValue + 1;
      setValue( uValue, true );
      
      break;
      }

    case KEY_SUB:
    case KEY_DOWN:
      {
      uint32_t uValue = m_uValue;
      if( uValue > 0 )
        uValue--;
      
      setValue( uValue, true );
      
      break;
      }

    case KEY_ESCAPE:
      {
      if( true == m_bEditing )
        {
        m_uValue = m_uOrgValue;
        }
      }

    case KEY_ENTER:
      {
      if( EDT_BUTTON == m_pEditor->uType )
        {
        GUISendMessage( m_pEditor->uMin, 0, m_pEditor->uMax);
        }
      else if( true == m_bEditing )
        {
        setEditing( false );
        }

      break;
      }

    default:
      {
      break;
      }
    }
}
//-----------------------------------------------------------------------------
// 定时消息
void TEditor::onTick( uint32_t uTick )
{
  
  if( true == m_bFocused && true == m_bEditing )
    {
    if( 0 == m_uLastTick )
      {
      m_uLastTick = uTick;
      }
    else
      {
      uint32_t uDiffTicks = uTick - m_uLastTick;
      if( 300 < uDiffTicks )
        {
        m_bTwinkle = bool(false == m_bTwinkle);

        drawEditor( false );
        }
      }
    }
}
//-----------------------------------------------------------------------------
