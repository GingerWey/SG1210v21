//-----------------------------------------------------------------------------
/*
   File        : ST7789S.h
   Version     : V1.2
   By          : 银网科技

   Description : ST7789S液晶驱动
          
   Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#ifndef LCD_ST7789S_H
#define LCD_ST7789S_H

#include "gpio.h"

#include "stm32f4xx_hal.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
 extern "C" {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// FSMC 16位总线模式下，HADDR[25:1]与FSMC_A[24:0]对应相连，HADDR【0】未接
// FSMC_A16口线对应物理地址A17
//
#define  LCD_BASE        ((uint32_t)0x6009FFFE)    // Bank1.Section1
#define  LCD_REGADDR    *(volatile uint16_t*)(LCD_BASE)             // A16 = nCS
#define  LCD_DATADDR    *(volatile uint16_t*)(LCD_BASE | (1 << 18)) // A17 = RS
//-----------------------------------------------------------------------------
#define  LCD_SendDumy   //__NOP(); __NOP(); __NOP(); __NOP()
//-----------------------------------------------------------------------------
//#define  LCD_WRAM_Perpare()   LCD_WriteReg( 0x2C )  //ST7789S_CMD_RAMWR )
#define  LCD_WRAM_Perpare()     LCD_REGADDR = 0x2C
//=============================================================================
// 全局数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
//void LCD_ST7789S_Setup(void);
void LCD_ST7789S_Init (void);

void LCD_ST7789S_DisplayOn (void);
void LCD_ST7789S_DisplayOff(void);

//void LCD_ST7789S_nCS_Set(int iState);

void LCD_ST7789S_SetCursor( uint32_t uX1, uint32_t uY1 );
void LCD_ST7789S_SetWindow( uint32_t uX1, uint32_t uY1, 
                            uint32_t uX2, uint32_t uY2 );

//void LCD_ST7789S_FillRect ( uint32_t uColor,
//                           uint32_t uX0, uint32_t uY0, 
//                           uint32_t uX1, uint32_t uY1 );

//void LCD_ST7789S_RectXOR( uint16_t uColor, 
//                          uint32_t uX0, uint32_t uY0, 
//                          uint32_t uX1, uint32_t uY1 );

void     LCD_WriteReg      ( uint16_t uwReg );
void     LCD_WriteRegValue ( uint16_t uwReg, uint16_t uwValue );
void     LCD_WriteData     ( uint16_t uwData );

uint32_t LCD_ReadReg  (uint32_t  uReg);
uint32_t LCD_ReadData (void);
uint32_t LCD_ReadId   (void);
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* LCD_ST7789S_H */
//-----------------------------------------------------------------------------
