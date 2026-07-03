// ------------------------------------------------------------
/*
   ImageRes.h
   CSG image data

   Encoder    name: CSG Toolkits
   Encoder version: 1.6.2627

   By Wey.   Silver Grid 2026
   Time: 2026-07-03 16:16:01
*/
// ------------------------------------------------------------
#ifndef ImageRes_H
#define ImageRes_H

#include "GUIPicture.h"

// ============================================================
#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------
// CSG Image: picbkg320x240csg
extern const TGUIPicture picbkg320x240csg;

#define IMAGE_BACKGROUND  &picbkg320x240csg
//===============================================================
/*
CSG atlas: picMAUAtlascsg
   No. Image                  resolution     size  CRM       CRN  CAS          Size  ratio
 --------------------------------------------------------------------------------------------
   1.  MA_Logo78x18Green.png     78x18       2134  RGB565     8  DEFLATE        648   30.37%
   2.  MA_Logo80x20Cyan.png      90x20       2189  RGB565     8  DEFLATE        772   35.27%
   3.  MA_ACPow32x59Cyan.png     32x59       4510  RGB565     8  MiniLZ77       276    6.12%
   4.  MA_CtrlG80x62Cyan.png     80x62       7635  RGB565    16  DEFLATE       1132   14.83%
   5.  MA_CtrlLED13x13Red.png    13x13        486  RGB565     8  RLE            104   21.40%
   6.  MA_Brkr56x60Cyan.png      56x60       8300  RGB565    16  DEFLATE       1564   18.84%
   7.  MA_Battery41x26C1.png     41x26       1695  RGB565    32  RLE            536   31.62%
   8.  MA_Battery41x26C2.png     41x26       1712  RGB565    32  RLE            540   31.54%
   9.  MA_Battery41x26C3.png     41x26       1683  RGB565    32  RLE            548   32.56%
  10.  MA_Battery41x26C4.png     41x26       1648  RGB565    64  RLE            676   41.02%
  11.  MA_Fan16x16Cyan.png       16x16        662  RGB565     4  MiniLZ77       116   17.52%
  12.  MA_Fire16x16.png          16x16        672  RGB565    16  RLE            160   23.81%
  13.  MU_Home24x22.png          24x22        868  RGB565     4  MiniLZ77       156   17.97%
  14.  MU_Item32x32_01.png       32x32       1887  RGB565     4  MiniLZ77       308   16.32%
  15.  MU_Item32x32_02.png       32x32       1364  RGB565     4  MiniLZ77       232   17.01%
  16.  MU_Item32x32_03.png       32x32       1592  RGB565     4  MiniLZ77       248   15.58%
  17.  MU_Item32x32_04.png       32x32       1533  RGB565     4  MiniLZ77       296   19.31%
  18.  MU_Item32x32_05.png       32x32       1235  RGB565     4  MiniLZ77       200   16.19%
  19.  MU_Item32x32_06.png       32x32       1755  RGB565     4  MiniLZ77       268   15.27%
  20.  MU_Item32x32_07.png       32x32       2025  RGB565     4  MiniLZ77       296   14.62%
  21.  MU_Item32x32_08.png       32x32       1935  RGB565     4  MiniLZ77       288   14.88%
  22.  MU_Item32x32_09.png       32x32       1299  RGB565     4  MiniLZ77       184   14.16%
  23.  MU_Item32x32_10.png       32x32       1903  RGB565     4  MiniLZ77       284   14.92%
 --------------------------------------------------------------------------------------------
   Total                                    50722                              9832   19.38%
*/
extern const TGUIPicture picMAUAtlascsg;

constexpr int picIdxMA_Logo78x18Green  = 0;
constexpr int picIdxMA_Logo80x20Cyan   = 1;
constexpr int picIdxMA_ACPow32x59Cyan  = 2;
constexpr int picIdxMA_CtrlG80x62Cyan  = 3;
constexpr int picIdxMA_CtrlLED13x13Red = 4;
constexpr int picIdxMA_Brkr56x60Cyan   = 5;
constexpr int picIdxMA_Battery41x26C1  = 6;
constexpr int picIdxMA_Battery41x26C2  = 7;
constexpr int picIdxMA_Battery41x26C3  = 8;
constexpr int picIdxMA_Battery41x26C4  = 9;
constexpr int picIdxMA_Fan16x16Cyan    = 10;
constexpr int picIdxMA_Fire16x16       = 11;
constexpr int picIdxMU_Home24x22       = 12;
constexpr int picIdxMU_Item32x32_01    = 13;
constexpr int picIdxMU_Item32x32_02    = 14;
constexpr int picIdxMU_Item32x32_03    = 15;
constexpr int picIdxMU_Item32x32_04    = 16;
constexpr int picIdxMU_Item32x32_05    = 17;
constexpr int picIdxMU_Item32x32_06    = 18;
constexpr int picIdxMU_Item32x32_07    = 19;
constexpr int picIdxMU_Item32x32_08    = 20;
constexpr int picIdxMU_Item32x32_09    = 21;
constexpr int picIdxMU_Item32x32_10    = 22;
//===============================================================

#ifdef __cplusplus
}
#endif
// ============================================================
#endif  // ImageRes_H
