//---------------------------------------------------------------------------
/*
  GUI Picture

  Display GUI picture

  Silver Grid Technology
  Date       : 2023.12.28
*/
//---------------------------------------------------------------------------
#include "GUIPicture.h"

#include "GUIBitMap.h"

#include "GUI.h"
#include "..\csgimage\CSGDecoder.h"
#include <vector>
//===========================================================================
// Global methods
//---------------------------------------------------------------------------
// Draw Picture
void GUI_DrawPicture(const TGUIPicture *pPic, int x0, int y0)
{
  
  if( nullptr == pPic || nullptr == pPic->pData)
    return ;
    
  switch( pPic->Type )
    {
    case ID_BITMAP:
      {
      const TGUIBitmap *pBmp = (const TGUIBitmap*)(pPic->pData);
      
      LCDX_Bitmap_Draw( pBmp, x0, y0 );
      
      break;
      }

//    case ID_PNG:
//      {
//      GUI_PNG_Draw( pPic->pData, pPic->Size, x0, y0 );
//      break;
//      }


    case ID_CSG:
      {
      CSGDecoder decoder;
      DecoderResult r = decoder.DecodePicture(
          static_cast<const uint8_t*>(pPic->pData), pPic->Size);
      if (r.error == CSG_ErrCode::kOk && !r.pixels.empty()) {
          const uint8_t* px = r.pixels.data();
          for (int y = 0; y < r.height; ++y) {
              for (int x = 0; x < r.width; ++x) {
                  uint8_t red   = *px++;
                  uint8_t green = *px++;
                  uint8_t blue  = *px++;
                  uint8_t alpha = *px++;
                  if (alpha > 0) {
                      GUI_SetColor((red << 16) | (green << 8) | blue);
                      GUI_DrawPixel(x0 + x, y0 + y);
                  }
              }
          }
      }
      break;
      }

    case ID_JPG:
      {
      GUI_JPEG_Draw( pPic->pData, pPic->Size, x0, y0 );
      break;
      }
    }
}
//---------------------------------------------------------------------------
