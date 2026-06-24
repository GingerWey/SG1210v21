//-----------------------------------------------------------------------------
/*
 File        : GUIHeap.h
 Version     : V1.10
 By          : 银网科技

 Description :管理GUI Heap内存分配的C访问接口
        
 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#ifndef __GUIHEAP_H
#define __GUIHEAP_H

#include <stddef.h>
#include <stdint.h>

#ifndef __vmSIMULATOR__
#include <cmsis_os.h>
#endif
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------

//=============================================================================
// 全局数据 
//-----------------------------------------------------------------------------

//=============================================================================
// 公用方法
//-----------------------------------------------------------------------------
// 初始化
#ifndef __vmSIMULATOR__
size_t GUI_GetFreeHeapSize( void );
size_t GUI_MinimumEverFreeHeapSize( void );
void   GUI_GetHeapStats ( HeapStats_t *pxHeapStats );
  
/*
 * Called automatically to setup the required heap structures the first time
 * SDGUIHEAP_Malloc() is called.
 */
void GUIHEAP_Init(void);

// 分配内存
void *GUIHEAP_Malloc( size_t stWantedSize);
// 释放内存块
void  GUIHEAP_Free( void *pv );

#else
#define   GUIHEAP_Init()                   {}

#define   RAM_GetFreeHeapSize()             0
#define   RAM_MinimumEverFreeHeapSize()     0

#define   RAM_Malloc(x)        (new char[x])
#define   RAM_Free(x)          (delete[] (x))
#endif

//=============================================================================
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif // __GUIHEAP_H
