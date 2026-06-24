//-----------------------------------------------------------------------------
/*
 File        : CSGDraw.h
 Version     : V1.00
 By          : Wey. Silver Grid

 Description : CSG image drawing -- MCU-safe streaming decoder integration.
               Uses CsgDecodeInit / CsgDecodePixels (no dynamic vectors).
               Memory allocated from emWin GUIHEAP, freed after drawing.
               Supports atlas sub-pictures and palette replacement.

 Date        : 2026.06.24
*/
//-----------------------------------------------------------------------------
#ifndef GUI_CSGDRAW_H
#define GUI_CSGDRAW_H

#include "GUIPicture.h"
#include <cstdint>

//-----------------------------------------------------------------------------
// Line buffer size -- must be ≥ max image width × max bytes-per-color (4 for RGBA)
// 256 pixels × 4 bytes = 1024 bytes
//-----------------------------------------------------------------------------
#ifndef CSG_LINEBUF_SIZE
  #define CSG_LINEBUF_SIZE  1024
#endif

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Public API
//=============================================================================

/// Draw a CSG image (single picture or sub-picture #0 of an atlas).
/// Memory: allocates line buffer from GUIHEAP, frees before return.
///
/// @param pPic   TGUIPicture with Type=ID_CSG, pData=CSG bytes, Size=byte count
/// @param x0     Left coordinate on LCD
/// @param y0     Top coordinate on LCD
void CSG_DrawPicture(const TGUIPicture* pPic, int x0, int y0);

/// Draw a specific sub-picture from a CSG atlas, with optional palette override.
///
/// @param pPic      TGUIPicture with Type=ID_CSG
/// @param picIndex  0-based picture index within the atlas
/// @param palette   New palette in CRM format, or nullptr to use embedded palette
/// @param x0, y0    Position on LCD
void CSG_DrawPictureEx(const TGUIPicture* pPic,
                       int picIndex,
                       const uint8_t* palette,
                       int x0, int y0);

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif  // GUI_CSGDRAW_H
