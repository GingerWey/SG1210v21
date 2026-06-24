//----------------------------------------------------------------------------
//
// 定义LCD绘制图案的控制结构
// 
// 银网科技
// 2015.1.3
//
//----------------------------------------------------------------------------
#ifndef GUI_Partten_H
#define GUI_Partten_H

#include "TypeDefine.h"
#include "GUIBitMap.h"
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// 图案的颜色深度 
#define PCD_1Bit        1	// 单色
#define PCD_8Gray       8	// 8位灰度
#define PCD_16RGB      16	// 16位RGB，16位分配：R(5)G(6)B(5)
#define PCD_24RGB      24	// 24位RGB, 按R,G,B顺序，各一字节
//-----------------------------------------------------------------------------
// 透明方式
#define PTM_BACKGROUND(x)   ((x) & 0xF)      // 对背景的处理方法
#define PTM_EMPTY       0       // 透明。多色单色图案: =uTColor时不填；	单色为0时不填
#define PTM_REPLACE     1       // 转换。多色图案: =uTColor时填uFColor；单色同PTM_FILL
#define PTM_FILL        2	// 不透明。多色图案用原色; 单色：0=uTColor, 1=uFColor

#define PTM_GETFORNT(x) ((x) & 0xF0)  // 前景的处理方法
#define PTM_MASK     0x10	// 只有于单色图案中，当点为1时用MaskData阵列中对应的点来替换
#define PTM_ALPHA    0x20       // 用于16/24bit图案，!=uTColor时，按ucAlpha混合uFColor
//=============================================================================
// 数据结构
//-----------------------------------------------------------------------------
// 单色图案绘制特性描述
// 可用于找绘制文字、图案、或图片
typedef struct tagGUIPartten
{
   INT8U    ucDeep;         // 图案的颜色深度
   INT8U    ucAlpha;        // 图案与背景混合的透明度
   INT8U    ucTransparent;  // 对图案透明的处理方法：
                            //   单色图案：为0的点不绘制
                            //   其它：    = uw0Color的点不绘制
   INT8U    ucReserved;   
  
  // 描述图案的定义数据
 const INT8U *pucData;            // 图案数据
	INT16U    uwWidth;        // 图案的宽度(bits)
	INT16U    uwHeight;       // 图案的高度(bits)

  // 输出描述
	INT16     wX;             // 绘制左上角x坐标
	INT16     wY;             // 绘制左上角y坐标

	// 下面4个数据描述对图案输出时的剪裁
	INT16U    uwLeft;         // 左
	INT16U    uwTop;          // 上
	INT16U    uwRight;        // 右
	INT16U    uwBottom;       // 下

	// 以下前景和透明颜色，定义见PTM_xxx宏描述
	INT32U    uFColor;        // 8位灰度图案使用此颜色为100%
	INT32U    uTColor;        // 0时输出的颜色；多位图案输入时，为透明色

	// 掩码
	INT16U    uwMaskWidth;	  // 掩码宽
	INT16U    uwMaskHeight;	  // 掩码高
 const TGUIBitmap *pbmMask;       // 掩码数据
} TGUIPartten;
//-----------------------------------------------------------------------------
#endif // __TGUIPartten_H
