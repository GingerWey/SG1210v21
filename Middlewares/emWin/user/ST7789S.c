//-----------------------------------------------------------------------------
/*
   File        : ST7789S.h
   Version     : V1.2
   By          : 银网科技

   Description : ST7789S晶驱动
          
   Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "ST7789S.h"  
  
#include "GUI.h"
#include "GUIConf.h"

#include "fsmc.h"
#include "RamHeap.h"
#include "gpio.h"

#include <stdint.h>
#include <cmsis_os.h>
//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------
  #if DESKTOP_WIDTH != DESKTOP_WIDTH
  #error  "DESKTOP_WIDTH != DESKTOP_WIDTH"
#endif
//-----------------------------------------------------------------------------
#ifndef LCD_nCS_HIGH
  #define LCD_nCS_HIGH
#endif

#ifndef LCD_nCS_LOW
  #define LCD_nCS_LOW
#endif

#ifndef LCD_RS_HIGH
  #define LCD_RS_HIGH
#endif

#ifndef LCD_RS_LOW
  #define LCD_RS_LOW
#endif
//-----------------------------------------------------------------------------
#define ABS(X)  ((X) > 0 ? (X) : -(X))    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define ST7789S_CMD_SWRESET            0x01   
#define ST7789S_CMD_LCD_ID             0x04   
#define ST7789S_CMD_SLEEP_IN           0x10 // Sleep in
#define ST7789S_CMD_SLEEP_OUT          0x11 // Sleep Out
#define ST7789S_CMD_PARTIAL_DISPLAY    0x12
#define ST7789S_CMD_NORMAL_DISPLAY     0x13
#define ST7789S_CMD_DISPLAY_INVERSIOFF 0x20 // Display Inversion Off
#define ST7789S_CMD_DISPLAY_INVERSION  0x21 // Display Inversion On
#define ST7789S_CMD_GAMSET             0x26 // display inversion set command
#define ST7789S_CMD_DISPOFF            0x28 // display off command
#define ST7789S_CMD_DISPLAY_ON         0x29 // Display On
#define ST7789S_CMD_CASET              0x2A // Column Address Set
#define ST7789S_CMD_RASET              0x2B // Row Address Set
#define ST7789S_CMD_WRITE_RAM          0x2C // Memory Write
#define ST7789S_CMD_READ_RAM           0x2E // Memory Read
#define ST7789S_CMD_VSCRDEF            0x33 // Vertical Scroll Definition
#define ST7789S_CMD_VSCSAD             0x37 // Vertical Scroll Start Address of RAM
#define ST7789S_CMD_TEARING_EFFECT     0x35
#define ST7789S_CMD_MADCTL             0x36
#define ST7789S_CMD_IDLE_MODE_OFF      0x38
#define ST7789S_CMD_IDLE_MODE_ON       0x39
#define ST7789S_CMD_COLOR_MODE         0x3A
#define ST7789S_CMD_WRCACE             0x55 // Write Content Adaptive Brightness Control and Color Enhancement
#define ST7789S_CMD_PORCH_CTRL         0xB2
#define ST7789S_CMD_GATE_CTRL          0xB7
#define ST7789S_CMD_VCOM_SET           0xBB
#define ST7789S_CMD_DISPLAY_OFF        0xBD
#define ST7789S_CMD_LCM_CTRL           0xC0
#define ST7789S_CMD_VDV_VRH_EN         0xC2
#define ST7789S_CMD_VRH_SET            0xC3
#define ST7789S_CMD_VDV_SET            0xC4
#define ST7789S_CMD_VCOMH_OFFSET_SET   0xC5
#define ST7789S_CMD_FR_CTRL            0xC6
#define ST7789S_CMD_ERGVAL_SELECTION1  0xC8 // Register Value Selection 1
#define ST7789S_CMD_ERGVAL_SELECTION2  0xCA // Register Value Selection 2
#define ST7789S_CMD_POWER_CTRL         0xD0
#define ST7789S_CMD_PV_GAMMA_CTRL      0xE0
#define ST7789S_CMD_NV_GAMMA_CTRL      0xE1 
//=============================================================================
// 本地方法
//-----------------------------------------------------------------------------
static void LCD_Delay( int iUSec )
{
  __IO int iCntr = iUSec * 12;
  while( --iCntr > 0 )
    __NOP();
}
//-----------------------------------------------------------------------------
//static void _SetupGPIO()
//{

//#ifdef LCD_nCS_Pin
//  {
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  LCD_nCS_HIGH;
//  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.Pin   = LCD_nCS_Pin;
//  HAL_GPIO_Init(LCD_nCS_Port, &GPIO_InitStruct);
//  }
//#endif

//#ifdef LCD_RS_Pin
//  {
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  LCD_RS_HIGH;
//  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.Pin   = LCD_RS_Pin;
//  HAL_GPIO_Init(LCD_RS_Port, &GPIO_InitStruct);
//  }
//#endif
//}
//-----------------------------------------------------------------------------
static void _SetupRegisters()
{
  
//  // Sleep In Command
//  LCD_WriteReg ( ST7789S_CMD_SLEEP_IN );   // 0x10
//  osDelay( 10 );

//  // SW Reset Command
//  LCD_WriteReg ( ST7789S_CMD_SWRESET );    // 0x01
//  osDelay( 200 );

  // Sleep Out Command
  LCD_WriteReg ( 0x11 ); //ST7789S_CMD_SLEEP_OUT );  // 0x11
  osDelay( 150 );

  // --------- ST7789S Frame rate setting 
  // PORCH control setting
  LCD_WriteReg ( ST7789S_CMD_PORCH_CTRL ); // 0xB2
  LCD_WriteData(0x0C);
  LCD_WriteData(0x0C);
  LCD_WriteData(0x00);
  LCD_WriteData(0x33);
  LCD_WriteData(0x33);

  // GATE control setting
  LCD_WriteReg ( ST7789S_CMD_GATE_CTRL );  // 0xB7
  LCD_WriteData( 0x35 );

  // --------- ST7789S Power setting
  // VCOM setting
  LCD_WriteReg ( ST7789S_CMD_VCOM_SET );   // 0xBB
  LCD_WriteData( 0x2B );
  
  // LCM Control setting
  LCD_WriteReg ( ST7789S_CMD_LCM_CTRL );   // 0xC0
  LCD_WriteData( 0x2C );
  
  // VDV and VRH Command Enable
  LCD_WriteReg ( ST7789S_CMD_VDV_VRH_EN ); // 0xC2
  LCD_WriteData( 0x01 );
  
  // VRH Set
  LCD_WriteReg ( ST7789S_CMD_VRH_SET );    // 0xC3
  LCD_WriteData( 0x17 );
  
  // VDV Set
  LCD_WriteReg ( ST7789S_CMD_VDV_SET );    // 0xC4
  LCD_WriteData( 0x20 );
  
  // Frame Rate Control in normal mode
  LCD_WriteReg ( ST7789S_CMD_FR_CTRL );    // 0xC6
  LCD_WriteData( 0x0F );
   
  // --------- Enable Register Value Selection
  // Register Value Selection 2
  LCD_WriteReg ( ST7789S_CMD_ERGVAL_SELECTION2 );  // 0xCA
  LCD_WriteData( 0x0F );
  
  // Register Value Selection 1
  LCD_WriteReg ( ST7789S_CMD_ERGVAL_SELECTION1 );  // 0xC8
  LCD_WriteData( 0x08 );
  
  // Write Content Adaptive Brightness Control and Color Enhancement
  LCD_WriteReg ( ST7789S_CMD_WRCACE );     // 0x55
  LCD_WriteData( 0x90 );
  
  // Power Control
  LCD_WriteReg ( ST7789S_CMD_POWER_CTRL ); // 0xD0
  LCD_WriteData( 0xA4 );
  LCD_WriteData( 0xA1 );
  
  // Color mode 16bits/pixel
  LCD_WriteReg ( ST7789S_CMD_COLOR_MODE );  // 0x3A
  LCD_WriteData( 0x05 ); // RGB565

  // Normal display for Driver Down side
  LCD_WriteReg ( ST7789S_CMD_MADCTL );      // 0x36
  LCD_WriteData( 0x08 ); // 
  
  // --------- ST7789S Gamma setting
  // Positive Voltage Gamma Control
  LCD_WriteReg ( ST7789S_CMD_PV_GAMMA_CTRL );  // 0xE0
  LCD_WriteData( 0xF0 );
  LCD_WriteData( 0x00 );
  LCD_WriteData( 0x0A );
  LCD_WriteData( 0x10 );
  LCD_WriteData( 0x12 );
  LCD_WriteData( 0x1b );
  LCD_WriteData( 0x39 );
  LCD_WriteData( 0x44 );
  LCD_WriteData( 0x47 );
  LCD_WriteData( 0x28 );
  LCD_WriteData( 0x12 );
  LCD_WriteData( 0x10 );
  LCD_WriteData( 0x16 );
  LCD_WriteData( 0x1B );
  
  // Negative Voltage Gamma Control
  LCD_WriteReg ( ST7789S_CMD_NV_GAMMA_CTRL );  // 0xE1
  LCD_WriteData( 0xF0 );
  LCD_WriteData( 0x00 );
  LCD_WriteData( 0x0A );
  LCD_WriteData( 0x10 );
  LCD_WriteData( 0x11 );
  LCD_WriteData( 0x1A );
  LCD_WriteData( 0x3B );
  LCD_WriteData( 0x34 );
  LCD_WriteData( 0x4E );
  LCD_WriteData( 0x3A );
  LCD_WriteData( 0x17 );
  LCD_WriteData( 0x16 );
  LCD_WriteData( 0x21 );
  LCD_WriteData( 0x22 );
  
  // --------- ST7789S  Display ON
  // Display ON command
  LCD_WriteReg( ST7789S_CMD_DISPLAY_ON ); // 0x29
  osDelay( 100 );
  
  LCD_WriteReg( ST7789S_CMD_WRITE_RAM );  // 0x2C Memory write
}
//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void LCD_ST7789S_Init(void)
{

#ifdef LCD_nCS_Pin
  _SetupGPIO();
#endif

  // 
  LCD_nRST_HIGH;
  osDelay( 5 );
  
  LCD_nRST_LOW;
  osDelay( 50 );

//  MX_FMC_SetLCD_WR();

  LCD_nRST_HIGH;
  osDelay( 150 );
  
  uint32_t uID = LCD_ReadId();
  
//  if( 0x7789 == uID )
    // 初始化控制器
    _SetupRegisters();

//  // Clear screen
//  LCD_ST7789S_FillRect( 0x001F, 0, 0,  DESKTOP_HEIGHT - 1, DESKTOP_WIDTH - 1 );
}
//-----------------------------------------------------------------------------
/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void LCD_ST7789S_DisplayOn(void)
{

  LCD_WriteReg( ST7789S_CMD_DISPLAY_ON );  // Display on
  osDelay( 100 );
}
//-----------------------------------------------------------------------------
/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void LCD_ST7789S_DisplayOff(void)
{
  // Display Off 
  LCD_WriteReg( ST7789S_CMD_DISPLAY_OFF );
}
//-----------------------------------------------------------------------------
/**
  * @brief  Sets the cursor position.
  * @param  wX0: specifies the X position.
  * @param  wY0: specifies the Y position. 
  * @retval None
  */
void LCD_ST7789S_SetCursor(uint32_t uX1, uint32_t uY1)
{

//  LCD_WriteReg ( ST7789S_CMD_CASET );   // Column address set
//  LCD_WriteData( uX1 >> 8);             // Xstart
//  LCD_WriteData( uX1 & 0xFF);           // Xstart
//  LCD_WriteData( uX1 >> 8);             // Xend
//  LCD_WriteData( uX1 & 0xFF);           // Xend
//  LCD_SendDumy;

//  LCD_WriteReg ( ST7789S_CMD_RASET );   // Row address set
//  LCD_WriteData( uY1 >> 8 );            // Ystart
//  LCD_WriteData( uY1 & 0xFF );          // Ystart
//  LCD_WriteData( uY1 >> 8 );            // Yend  
//  LCD_WriteData( uY1 & 0xFF );          // Yend  
//  LCD_SendDumy;

//  LCD_WriteReg ( ST7789S_CMD_WRITE_RAM );    // 0x2C Memory write
//  LCD_SendDumy;
  
  LCD_REGADDR = ST7789S_CMD_CASET;
  LCD_DATADDR = uX1 >> 8;
  LCD_DATADDR = uX1 & 0xFF;
  LCD_DATADDR = (DESKTOP_WIDTH - 1) >> 8;
  LCD_DATADDR = (DESKTOP_WIDTH - 1) & 0xFF;

  LCD_REGADDR = ST7789S_CMD_RASET;
  LCD_DATADDR = uY1 >> 8;
  LCD_DATADDR = uY1 & 0xFF;
  LCD_DATADDR = (DESKTOP_HEIGHT - 1) >> 8;
  LCD_DATADDR = (DESKTOP_HEIGHT - 1) & 0xFF;

  LCD_REGADDR = ST7789S_CMD_WRITE_RAM;  // 0x2C Memory write
}
//-----------------------------------------------------------------------------
void LCD_ST7789S_SetWindow( uint32_t uX1, uint32_t uY1, uint32_t uX2, uint32_t uY2 )
{
  
//  LCD_WriteReg ( ST7789S_CMD_CASET );   // Column address set
//  LCD_WriteData( uX1 >> 8);             // Xstart
//  LCD_WriteData( uX1 & 0xFF);           // Xstart
//  LCD_WriteData( uX2 >> 8);             // Xend
//  LCD_WriteData( uX2 & 0xFF);           // Xend
//  LCD_SendDumy;

//  LCD_WriteReg ( ST7789S_CMD_RASET );   // Row address set
//  LCD_WriteData( uY1 >> 8 );            // Ystart
//  LCD_WriteData( uY1 & 0xFF );          // Ystart
//  LCD_WriteData( uY2 >> 8 );            // Yend  
//  LCD_WriteData( uY2 & 0xFF );          // Yend  
//  LCD_SendDumy;

//  LCD_WriteReg ( ST7789S_CMD_WRITE_RAM );    // 0x2C Memory write
//  LCD_SendDumy;

  LCD_REGADDR = ST7789S_CMD_CASET;
  LCD_DATADDR = uX1 >> 8;
  LCD_DATADDR = uX1 & 0xFF;
  LCD_DATADDR = uX2 >> 8;
  LCD_DATADDR = uX2 & 0xFF;

  LCD_REGADDR = ST7789S_CMD_RASET;
  LCD_DATADDR = uY1 >> 8;
  LCD_DATADDR = uY1 & 0xFF;
  LCD_DATADDR = uY2 >> 8;
  LCD_DATADDR = uY2 & 0xFF;
  
  LCD_REGADDR = ST7789S_CMD_WRITE_RAM;  // 0x2C Memory write
}
//-----------------------------------------------------------------------------
void LCD_ST7789S_FillRect ( uint32_t uColor,
                           uint32_t uX0, uint32_t uY0, 
                           uint32_t uX1, uint32_t uY1 )
{

  LCD_ST7789S_SetWindow( uX0, uY0, uX1, uY1 );
  
  LCD_RS_HIGH;
  LCD_nCS_LOW;
  
  int iNumPixels = (uX1 - uX0 + 1) * (uY1 - uY0 + 1);
  while( iNumPixels-- > 0 )
    {
    LCD_DATADDR = uColor;
    }

  LCD_nCS_HIGH;
//  for( uint32_t uY = uY0; uY <= uY1; uY++ )
//    {
//    LCD_ST7789S_SetCursor( uX0, uY );
//    for( uint32_t uX = uX0; uX <= uX1; uX++ )
//      {
//      LCD_WriteData( uColor );
//      }
//    }
}
//-----------------------------------------------------------------------------
void LCD_ST7789S_RectXOR( uint16_t uColor, 
                          uint32_t uX0, uint32_t uY0, 
                          uint32_t uX1, uint32_t uY1)
{
  
  LCD_ST7789S_SetWindow( uX0, uY0, uX1, uY1 );
    
  uint32_t  uHeight = uY1 - uY0 + 1;
  uint16_t *puwBuff = (uint16_t*)RAM_Malloc( uHeight * 2 );
#ifdef USE_DEV_ASSERT
   DEV_ASSERT( 0 == puwBuff, GFC_OutOfMem );
#endif

  for( uint32_t uX = uX0; uX <= uX1; uX++ )
    {
    for( uint32_t uIdx = 0; uIdx < uHeight; uIdx++ )
      {
      LCD_nCS_LOW;
      __IO uint16_t uwData = LCD_DATADDR;
      LCD_nCS_HIGH;
     
      puwBuff[uIdx] = uwData;
      }

    for( uint32_t uIdx = 0; uIdx < uHeight; uIdx++ )
      {
      LCD_WriteData( puwBuff[uIdx] ^ uColor );
      }
    }
    
  RAM_Free( puwBuff );
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void LCD_WriteReg ( uint16_t uwRegValue )
{
  
  LCD_RS_LOW;
  
  LCD_nCS_LOW;
  
  __NOP();
  
  LCD_SendDumy;
  LCD_REGADDR = uwRegValue;
  LCD_SendDumy;
  
  LCD_nCS_HIGH;
  
  LCD_RS_HIGH;
}
//-----------------------------------------------------------------------------
void LCD_WriteRegValue ( uint16_t uwReg, uint16_t uwValue )
{
  
  LCD_REGADDR = uwReg;
  
  LCD_DATADDR = uwValue;
}
//-----------------------------------------------------------------------------
void LCD_WriteData( uint16_t uwData )
{

  LCD_RS_HIGH;
  LCD_nCS_LOW;
  
  __NOP();
 
  LCD_SendDumy;
  LCD_DATADDR = uwData;
  LCD_SendDumy;
  
  LCD_nCS_HIGH;
}
//-----------------------------------------------------------------------------
uint32_t LCD_ReadReg  (uint32_t  uReg)
{

  LCD_WriteReg( uReg );
  
  for( int iIdx = 0; iIdx < 160 * 5; iIdx++ )
    __NOP();

  uint32_t uData = LCD_ReadData();
  
  return uData;
}
//-----------------------------------------------------------------------------
uint32_t LCD_ReadData (void)
{
  
  LCD_RS_HIGH;
  LCD_nCS_LOW;
  
  uint32_t uData = LCD_DATADDR;
  
  LCD_nCS_HIGH;
  
  return uData;
}
//-----------------------------------------------------------------------------
uint32_t LCD_ReadId   (void)
{
  
  osDelay( 50 );
  LCD_WriteRegValue ( 0x0000, 0x0001 );
  osDelay( 50 );

  uint32_t uIdent = LCD_ReadReg ( 0x0000 );
  osDelay( 50 );
  
  //读到ID不正确,新增uIdent==0x9300判断，因为9341在未被复位的情况下会被读成9300
  if ( uIdent < 0xFF || uIdent == 0xFFFF || uIdent == 0x9300 ) 
    {
    //尝试9341 ID的读取
    LCD_WriteReg ( 0xD3 );
    uIdent = LCD_ReadData();          //dummy read
    uIdent = LCD_ReadData();          //读到0x00
    uIdent = LCD_ReadData();          //读取93
    uIdent <<= 8;   
    uIdent |= LCD_ReadData();         //读取41
    if ( uIdent != 0x9341 )           //非9341,尝试是不是6804
      {     
      LCD_WriteReg ( 0x04 );     
      uIdent = LCD_ReadData();        //dummy read
      uIdent = LCD_ReadData();        //读到0x85
      uIdent = LCD_ReadData();        //读取0x85
      uIdent <<= 8;
      uIdent |= LCD_ReadData();       //读取0x52
      
      if ( uIdent == 0x8552 )         //将8552的ID转换成7789
        {   
        uIdent = 0x7789;    
        }
    
      else                            //也不是ST7789, 尝试是不是 NT35310
        {   
        LCD_WriteReg ( 0xBF );   
        uIdent = LCD_ReadData();      //dummy read
        uIdent = LCD_ReadData();      //读回0x01
        uIdent = LCD_ReadData();      //读回0xD0
        uIdent = LCD_ReadData();      //这里读回0x68
        uIdent <<= 8; 
        uIdent |= LCD_ReadData();     //这里读回0x04
        if ( uIdent != 0x6804 )       //也不是6804,尝试看看是不是NT35310
          { 
          LCD_WriteReg ( 0xD4 ); 
          uIdent = LCD_ReadData();    //dummy read
          uIdent = LCD_ReadData();    //读回0x01
          uIdent = LCD_ReadData();    //读回0x53
          uIdent <<= 8; 
          uIdent |= LCD_ReadData();   //这里读回0x10
          if ( uIdent != 0x5310 )     //也不是NT35310,尝试看看是不是NT35510
            {
            LCD_WriteReg ( 0xDA00 );
            uIdent = LCD_ReadData();  //读回0x00
            LCD_ReadReg ( 0xDB00 );
            uIdent = LCD_ReadData();  //读回0x80
            uIdent <<= 8;
            LCD_WriteReg ( 0xDC00 );
            uIdent |= LCD_ReadData(); //读回0x00
              
            // NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
            if ( uIdent == 0x8000 )
               uIdent = 0x5510;
            
            if ( uIdent != 0x5510 )   //也不是NT5510,尝试看看是不是SSD1963
              {
              LCD_WriteReg ( 0xA1 );
              uIdent = LCD_ReadData();
              uIdent = LCD_ReadData(); //读回0x57
              uIdent <<= 8;
              uIdent |= LCD_ReadData(); //读回0x61
                
              //SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
              if ( uIdent == 0x5761 ) 
                uIdent = 0x1963;
              else
                uIdent = 0;
              }
            }
          }
        }
      }
    }
    
  // 如果是这几个IC,则设置WR时序为最快
  if ( uIdent == 0x9341 || uIdent == 0x5310 || 
       uIdent == 0x5510 || uIdent == 0x1963 ) 
    {
    //重新配置写时序控制寄存器的时序
    FSMC_Bank1E->BWTR[6] &= ~ ( 0xF << 0 ); //地址建立时间(ADDSET)清零
    FSMC_Bank1E->BWTR[6] &= ~ ( 0xF << 8 ); //数据保存时间清零
    FSMC_Bank1E->BWTR[6] |= 3 << 0;         //地址建立时间(ADDSET)为3个HCLK =18ns
    FSMC_Bank1E->BWTR[6] |= 2 << 8;         //数据保存时间(DATAST)为6ns*3个HCLK=18ns
    }
    
  //6804/C505速度上不去,得降低
  else if ( uIdent == 0x6804 || uIdent == 0xC505 ) 
    {
    //重新配置写时序控制寄存器的时序
    FSMC_Bank1E->BWTR[6] &= ~ ( 0xF << 0 ); //地址建立时间(ADDSET)清零
    FSMC_Bank1E->BWTR[6] &= ~ ( 0xF << 8 ); //数据保存时间清零
    FSMC_Bank1E->BWTR[6] |= 10 << 0;        //地址建立时间(ADDSET)为10个HCLK =60ns
    FSMC_Bank1E->BWTR[6] |= 12 << 8;        //数据保存时间(DATAST)为6ns*13个HCLK=78ns
    }

  return uIdent;
}
//-----------------------------------------------------------------------------
