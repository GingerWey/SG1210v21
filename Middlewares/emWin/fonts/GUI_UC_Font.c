//-----------------------------------------------------------------------------
/*
  File        : GUI_UC_EncodeNoneX.c
  Version     : V1.10
  By          : 银网科技

  For         : emWin
  Mode        : Thumb2
  Description :扩展非ACSII的Uniocde字符显示方法

  Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "GUI_UC_Font.h"

#include "GUI.h"

#include "GUI_Private.h"
#include <LCD.h>
#include <GUI_Type.h>
#include <LCD_Protected.h>
#include <Global.h>
/*********************************************************************

        Static code

**********************************************************************
*/

const TGUIFontItem *GUI_UC_FindFontItem ( const TGUIFontList *pFontList,
    U16P ch )
{

  // 在小字库中搜索
  const TGUIFontItem *pfiChar = NULL;

  if ( 4 < pFontList->Count )
    {
    U32 ulCur = 0, ulBeg = 0, ulEnd = pFontList->Count - 1;

    while ( ulEnd - ulBeg > 1  )
      {
      ulCur = (ulBeg + ulEnd ) >> 1;

      const U8 *ucCode = pFontList->Items[ulCur].Char;
      U16 uwCode = (( ( U16 ) ucCode[0] ) << 8 ) + ucCode[1];

      if ( uwCode < ch )
        ulBeg = ulCur;
      else if ( uwCode > ch )
        ulEnd = ulCur;
      else
        {
        pfiChar = & ( pFontList->Items[ulCur]);
        break;
        }
      }

    //
    if ( NULL == pfiChar )
      {
      if ( ulCur == ulBeg )
        {
        const U8 *ucCode = pFontList->Items[ulEnd].Char;
        U16 uwCode = (( ( U16 ) ucCode[0] ) << 8 ) + ucCode[1];
        if ( uwCode == ch )
          pfiChar = & ( pFontList->Items[ulEnd]);
        }
      else
        {
        const U8 *ucCode = pFontList->Items[ulBeg].Char;
        U16 uwCode = (( ( U16 ) ucCode[0] ) << 8 ) + ucCode[1];
        if ( uwCode == ch )
          pfiChar = & ( pFontList->Items[ulBeg]);
        }
      }
    }
  else
    {
    // 字库中字符太少时，用线性搜索
    for ( int iIdx = pFontList->Count - 1; iIdx >= 0; iIdx-- )
      {
      const U8 *ucCode = pFontList->Items[iIdx].Char;
      U16 uwCode = (( ( U16 ) ucCode[0] ) << 8 ) + ucCode[1];

      if ( uwCode == ch )
        {
        pfiChar = & ( pFontList->Items[iIdx]);
        break;
        }
      }
    }

  return pfiChar;
}

// 搜索Unicode字符的字模
static const GUI_CHARINFO *GUI_UC_FindChar ( U16P ch, GUI_CHARINFO *pCharInfo )
{

  const GUI_FONT_PROP *pProp = GUI_pContext->pAFont->p.pProp;
  for ( ; NULL != pProp; pProp = pProp->pNext )
    {
    if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
      break;
    }

  const GUI_CHARINFO *pciChar = NULL;
  if ( NULL != pProp )
    {
    if ( 0xA1 > ch )
      {
      // UC字体中定义的ASCII是借用库的标准字体, 必须将CharInfo域强制转回来
      const GUI_FONT *pACS2Font = (const GUI_FONT*)(pProp->paCharInfo);
      for ( pProp = pACS2Font->p.pProp; NULL != pProp; pProp = pProp->pNext )
        {
        if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
          break;
        }

      if ( NULL != pProp )
        {
        pciChar = & ( pProp->paCharInfo[ch - pProp->First]);
        }
      }
    else if ( NULL != pCharInfo )
      {
      // 取回小字库指针
      const TGUIFontList *pUCFontList = (const TGUIFontList*)pProp->paCharInfo;

      // 在小字库中搜索需要的字符
      const TGUIFontItem *pfiChar = GUI_UC_FindFontItem ( pUCFontList, ch);
      if ( NULL != pfiChar )
        {
        pCharInfo->XSize = pUCFontList->Type.Width;
        pCharInfo->XDist = pUCFontList->Type.Width;
        pCharInfo->BytesPerLine = pUCFontList->Type.BytesPerLine;
        pCharInfo->pData = pfiChar->DotMix;

        pciChar = pCharInfo;
        }
      }
    }

  return pciChar;
}
/*********************************************************************

        Public code

**********************************************************************
*/
/*********************************************************************

        GUIPROP_DispChar

  Purpose:
    This is the routine that displays a character. It is used by all
    other routines which display characters as a subroutine.
*/
void GUIPROP_UC_DispChar ( U16P ch )
{


  GUI_CHARINFO FCharInfo;
  const GUI_CHARINFO *pciChar = GUI_UC_FindChar(ch, &FCharInfo);

  if ( NULL != pciChar )
    {
    int BytesPerLine = pciChar->BytesPerLine;

    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode;
    GUI_DRAWMODE OldDrawMode = LCD_SetDrawMode ( DrawMode);

    LCD_DrawBitmap ( GUI_pContext->DispPosX,     GUI_pContext->DispPosY,
                     pciChar->XSize,             GUI_pContext->pAFont->YSize,
                     GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                     1, /* Bits per Pixel */
                     BytesPerLine,
                     pciChar->pData,
                     &LCD_BKCOLORINDEX );

    // Fill empty pixel lines
    if ( GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize &&
         DrawMode != LCD_DRAWMODE_TRANS )
      {
      int YMag  = GUI_pContext->pAFont->YMag;
      int YDist = GUI_pContext->pAFont->YDist * YMag;
      int YSize = GUI_pContext->pAFont->YSize * YMag;
      LCD_COLOR OldColor = GUI_GetColor();
      GUI_SetColor ( GUI_GetBkColor());
      LCD_FillRect ( GUI_pContext->DispPosX,
                     GUI_pContext->DispPosY + YSize,
                     GUI_pContext->DispPosX + pciChar->XSize,
                     GUI_pContext->DispPosY + YDist);
      GUI_SetColor ( OldColor);
      }
    LCD_SetDrawMode ( OldDrawMode); /* Restore draw mode */

    GUI_pContext->DispPosX += pciChar->XDist * GUI_pContext->pAFont->XMag;
    }

}

/*********************************************************************

         GUIPROP_GetCharDistX
*/
int GUIPROP_UC_GetCharDistX ( U16P ch, int *pSizeX )
{

  const GUI_FONT_PROP *pProp = GUI_pContext->pAFont->p.pProp;
  for ( ; NULL != pProp; pProp = pProp->pNext )
    {
    if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
      break;
    }

  int iRes = 0;
  if ( NULL != pProp )
    {
    if ( 0xA1 > ch )
      {
      // UC字体中定义的ASCII是借用库的标准字体, 必须将CharInfo域强制转回来
      const GUI_FONT *pACS2Font = (const GUI_FONT*)(pProp->paCharInfo);
      for ( pProp = pACS2Font->p.pProp; NULL != pProp; pProp = pProp->pNext )
        {
        if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
          break;
        }

      if ( NULL != pProp )
        {
        const GUI_CHARINFO *pciChar = & ( pProp->paCharInfo[ch - pProp->First]);
        iRes = pciChar->XSize + pACS2Font->XMag;
        if ( NULL != pSizeX )
          *pSizeX = pciChar->XSize;
        }
      else
        {
        iRes = 6;
        if ( NULL != pSizeX )
          *pSizeX = 4;
        }
      }
    else
      {
      // ch是汉字
      // 取回小字库指针
      const TGUIFontList *pUCFontList = (const TGUIFontList*)pProp->paCharInfo;
      iRes = pUCFontList->Type.Width + GUI_pContext->pAFont->XMag;
      if ( NULL != pSizeX )
        *pSizeX = pUCFontList->Type.Width;
      }
    }

  return iRes;
}

/*********************************************************************

         GUIPROP_UC_GetFontInfo
*/
void GUIPROP_UC_GetFontInfo ( const GUI_FONT *pFont, GUI_FONTINFO *pfi )
{

  pfi->Flags = GUI_FONTINFO_FLAG_PROP;
  pfi->Baseline = pFont->Baseline;
  pfi->LHeight = pFont->LHeight;
  pfi->CHeight = pFont->CHeight;
}

/*********************************************************************

        GUIPROP_UC_IsInFont
*/
char GUIPROP_UC_IsInFont ( const GUI_FONT *pFont, U16 ch )
{

  const GUI_FONT_PROP *pProp = pFont->p.pProp;
  for ( ; NULL != pProp; pProp = pProp->pNext )
    {
    if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
      break;
    }

  char iRes = 0;
  if ( NULL != pProp )
    {
    if ( 0xA1 > ch )
      {
      // UC字体中定义的ASCII是借用库的标准字体, 必须将CharInfo域强制转回来
      const GUI_FONT *pACS2Font = (const GUI_FONT*)(pProp->paCharInfo);
      if ( NULL != pACS2Font->pfIsInFont )
        iRes = pACS2Font->pfIsInFont ( pACS2Font, ch);
      else
        {
        for ( pProp = pACS2Font->p.pProp; pProp; pProp = pProp->pNext )
          {
          if ( ( ch >= pProp->First ) && ( ch <= pProp->Last ) )
            break;
          }

        if ( NULL != pProp )
          iRes = 1;
        }
      }
    else
      {
      // ch是汉字
      // 取回小字库指针
      const TGUIFontList *pUCFontList = (const TGUIFontList*)pProp->paCharInfo;
      if ( NULL != GUI_UC_FindFontItem ( pUCFontList, ch ) )
        iRes = 1;
      }
    }

  //
  //for (; pProp; pProp = pProp->pNext) {
  //    if ((pProp->First <= ch) && (pProp->Last >= ch))
  //        break;
  //}

  //if (NULL != pProp)
  //  {
  //  if( ch < 0xA0 )
  //    {
  //    // UC字体中定义的ASCII是借用库的标准字体, 必须将CharInfo域强制转回来
  //    const GUI_FONT* pACS2Font = (const GUI_FONT*)(pProp->paCharInfo);
  //    for (pProp = pACS2Font->p.pProp; NULL != pProp; pProp = pProp->pNext)
  //      {
  //      if( (ch >= pProp->First) && (ch <= pProp->Last))
  //        break;
  //      }

  //    if (NULL != pProp)
  //      iRes = 1;
  //    }
  //  else if( 0 != GUI_UC_FindFontItem( pProp, ch ) )
  //    iRes = 1;
  //  }

  return iRes; /* No, we can not display this character */
}

/*********************************************************************

         GUIPROP_UC_GetCharInfo
*/
int GUIPROP_UC_GetCharInfo ( U16P ch, GUI_CHARINFO_EXT *pInfo )
{

  const GUI_FONT_PROP *pProp = GUI_pContext->pAFont->p.pProp;
  int iRes = 0;

  for ( ; NULL != pProp; pProp = pProp->pNext )
    {
    if ( ( pProp->First <= ch ) && ( pProp->Last >= ch ) )
      break;
    }

  if ( NULL != pProp )
    {
    if ( 0xA1 > ch )
      {
      // UC字体中定义的ASCII是借用库的标准字体, 必须将CharInfo域强制转回来
      const GUI_FONT *pACS2Font = (const GUI_FONT*)(pProp->paCharInfo);
      if ( pACS2Font->pfGetCharInfo )
        {
        iRes = pACS2Font->pfGetCharInfo ( ch, pInfo);
        }
      else
        {
        for ( pProp = pACS2Font->p.pProp; pProp; pProp = pProp->pNext )
          {
          if ( ( ch >= pProp->First ) && ( ch <= pProp->Last ) )
            break;
          }

        if ( NULL != pProp )
          {
          const GUI_CHARINFO *pciChar = &(pProp->paCharInfo[ch - pProp->First]);
          pInfo->XSize = pciChar->XSize;
          pInfo->YSize = pACS2Font->YSize;
          pInfo->XPos = 0;
          pInfo->YPos = 0;
          pInfo->XDist = pciChar->XDist;
          pInfo->pData = pciChar->pData;

          iRes = 1;
          }
        }
      }
    else
      {
      // 取回小字库指针
      const TGUIFontList *pUCFont = (const TGUIFontList*)pProp->paCharInfo;

      GUI_CHARINFO FCharInfo;
      const GUI_CHARINFO *pciChar = GUI_UC_FindChar(ch, &FCharInfo);
      if ( NULL != pciChar )
        {
        pInfo->XSize = pciChar->XSize;
        pInfo->YSize = pUCFont->Type.Height;
        pInfo->XPos = 0;
        pInfo->YPos = 0;
        pInfo->XDist = pciChar->XDist;
        pInfo->pData = pciChar->pData;
        iRes = 1;
        }
      }
    }

  return iRes;
}
