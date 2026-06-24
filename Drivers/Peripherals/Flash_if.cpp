/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/flash_if.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/** @addtogroup STM32F4xx_IAP
  * @{
  */
 
/* Includes ------------------------------------------------------------------*/
#define HAL_FLASH_MODULE_ENABLED
#include "flash_if.h"
#include "gpio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
  static int GetSector( volatile uint32_t Address);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  HAL_FLASH_Unlock(); 

#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)

  /* Clear pending flags (if any) */  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
#elif defined(STM32F103xE)
  /* Clear pending flags (if any) */  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                         FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
#endif
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
__weak void FLASH_EarseProgress(uint32_t uNo, uint32_t uCount)
{
}

uint32_t FLASH_If_Erase(uint32_t uNumBytes)
{

#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
  int UserStartSector, UserEndSector;

  // Clear pending flags (if any)  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

  // Get the sector where start the user flash area
  UserStartSector = GetSector(APPLICATION_ADDRESS);
  UserEndSector   = GetSector(APPLICATION_ADDRESS + uNumBytes - 1);
  if( UserEndSector < 0 || UserStartSector < 0 )
    return (1);
  
  for(int iIdx = UserStartSector; iIdx <= UserEndSector; iIdx++ )
    {
    // Device voltage range supposed to be [2.7V to 3.6V], 
    // the operation will be done by word 
    FLASH_Erase_Sector(iIdx, FLASH_VOLTAGE_RANGE_3);
    if ( HAL_FLASH_GetError() != HAL_OK )
      {
      // Error occurred while page erase
      return (1);
      }
    
    // 2018.5.12  Wey.
    FLASH_EarseProgress( iIdx - UserStartSector + 1, 
                         UserEndSector - UserStartSector + 1 );
    }
#elif defined(STM32F103xE)
  uint32_t uPageError;
  FLASH_EraseInitTypeDef EraseInit;
  EraseInit.Banks       = FLASH_BANK_1;
  EraseInit.PageAddress = APPLICATION_ADDRESS;
  EraseInit.NbPages     = (uNumBytes + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
  EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;  
    
  if( HAL_OK != HAL_FLASHEx_Erase( &EraseInit, &uPageError ) )
    return 1;
#endif

  return (0);
}

uint32_t FLASH_If_BlankCheck(uint32_t uNumBytes)
{
  
  int iIdx = ((uNumBytes + 3) / 4 )- 1;
  uint32_t uRes = 0;
  
  uint32_t *puMem = (uint32_t*)APPLICATION_ADDRESS;
  
  for( ; iIdx >= 0; iIdx-- )
    if( *puMem++ != (uint32_t)-1 )
      {
      uRes = (uint32_t)puMem;
      break;
      }
 
  return uRes;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  uFlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
//static void FLASH_Program_Word(uint32_t Address, uint32_t Data)
//{
//  /* Check the parameters */
//  assert_param(IS_FLASH_ADDRESS(Address));
//  
//  /* If the previous operation is completed, proceed to program the new data */
//  CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
//  FLASH->CR |= FLASH_PSIZE_WORD;
//  FLASH->CR |= FLASH_CR_PG;

//  *(__IO uint32_t*)Address = Data;
//}

uint32_t FLASH_If_Write(volatile uint32_t  uFlashAddress, 
                           const uint32_t* puData, 
                                 uint32_t  uNumInt32s )
{

  uint32_t uWaitCnt = 1000;
  while( uWaitCnt-- > 0 )
    if( __HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) == RESET ) 
      break;
    
  if( 0 == uWaitCnt )
    return (3);   // Time out
  
  __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR );
  
  __disable_irq();

  uint32_t uRes = 0;
  for( uint32_t uIdx = 0; uNumInt32s > 0; uNumInt32s--, uIdx++ )
    {
    if( 0 == IS_FLASH_ADDRESS(uFlashAddress) )
      {
      // Overflow
      uRes = 2;
      break;
      }
      
    // Device voltage range supposed to be [2.7V to 3.6V], 
    //  the operation will be done by word 
    //FLASH_Program_Word((uint32_t)uFlashAddress, Data[uIdx] );
    CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;
    
    *(__IO uint32_t*)uFlashAddress = puData[uIdx];
    
    __NOP();
    
    // Check the written value
    if( *(uint32_t*)uFlashAddress != puData[uIdx] )
      {
      // Flash content doesn't match SRAM content
      uRes = 3;
      break;
      }
      
    // Increment FLASH destination address
    uFlashAddress += sizeof(uint32_t);
    }
    
  __enable_irq();

  return uRes;
}
    
#elif defined(STM32F103xE)
uint32_t FLASH_If_Write(volatile uint32_t  uFlashAddress, 
                           const uint32_t* puData, 
                                 uint32_t  uNumInt32s )
{

  uint32_t uWaitCnt = 1000;
  while( uWaitCnt-- > 0 )
    if( __HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) == RESET ) 
      break;
    
  if( 0 == uWaitCnt )
    return (3);   // Time out
  
  __IO const uint16_t *puwData    = (__IO const uint16_t*)puData;
  __IO uint32_t        uNumInt16s = uNumInt32s * 2;
    
  /* Clear pending flags (if any) */  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                         FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
    
  for (int i = 0; (i < uNumInt16s) && ((uint32_t)uFlashAddress <= (CODE_FLASH_END_ADDRESS-2)); i++)
    {
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    *(__IO uint16_t*)uFlashAddress = puwData[i]; 
      
    if(  HAL_OK == FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE) ) 
    //if( HAL_OK == HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, (uint32_t)uFlashAddress, Data[i] ) )
      {
      /* Check the written value */
      if( *(uint16_t*)uFlashAddress != puwData[i] )
        {
        /* Flash content doesn't match SRAM content */
        return(2);
        }
      /* Increment FLASH destination address */
      uFlashAddress += sizeof(uint16_t);
      }
    else
      {
      /* Error occurred while writing data in Flash memory */
      return (1);
      }
  
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    }

  return (0);
}
#endif

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint32_t FLASH_If_GetWriteProtectionStatus(void)
{
  
  FLASH_OBProgramInitTypeDef OBInit;
  
  /* Check if there are write protected sectors inside the user flash area */
  HAL_FLASHEx_OBGetConfig( &OBInit );
  
#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx) 
  return OBInit.WRPSector;
#elif defined(STM32F103xE)
  return OBInit.WRPPage;
#endif
}

/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_DisableWriteProtection(void)
{
  
  FLASH_OBProgramInitTypeDef OBInit;

#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
  // Get the sector where start the user flash area
  int UserStartSector = GetSector(APPLICATION_ADDRESS);
  if( UserStartSector < 0 )
    return (1);

  // Mark all sectors inside the user flash area as non protected
  uint32_t UserWrpSectors = 0xFFF - ((1 << UserStartSector) - 1);
#elif defined(STM32F103xE)
  uint32_t UserWrpSectors = (1 << ((APPLICATION_ADDRESS - FLASH_BASE) / FLASH_SECTOR_SIZE)) - 1;  
#endif
  
  /* Unlock the Option Bytes */
  HAL_FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  OBInit.OptionType = OPTIONBYTE_WRP;
  OBInit.WRPState   = OB_WRPSTATE_DISABLE;
  OBInit.Banks      = FLASH_BANK_1;

#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx) 
  OBInit.WRPSector  = UserWrpSectors;
#elif defined(STM32F103xE)
  OBInit.WRPPage    = UserWrpSectors;  
#endif

  for( int iCntr = 20; iCntr > 0; iCntr-- )
    {
    if( HAL_OK == HAL_FLASHEx_OBProgram( &OBInit ) )
      break;

    HAL_Delay(1);
    
#if defined(STM32F427xx) | defined(STM32F429xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR));
#elif defined(STM32F407xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR));
#elif defined(STM32F103xE)
    /* Clear pending flags (if any) */  
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                           FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
#endif
    }

  HAL_FLASH_OB_Lock();
    
  // Write Protection successfully disabled
  return (0);
}

/**
  * @brief  Enables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully enabled
  *         1: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_EnableWriteProtection(void)
{
  
  //__IO uint32_t UserStartSector, UserWrpSectors = OB_WRP_SECTOR_1;
  FLASH_OBProgramInitTypeDef OBInit;
  int iCntr;

  /* Unlock the Option Bytes */
  //FLASH_OB_Unlock();
  HAL_FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  //FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);
  OBInit.OptionType = OPTIONBYTE_WRP;
  OBInit.WRPState   = OB_WRPSTATE_ENABLE;
  OBInit.Banks      = FLASH_BANK_1;
#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
  OBInit.WRPSector  = OB_WRP_SECTOR_All;
#elif defined(STM32F103xE)
  OBInit.WRPPage    = OB_WRP_ALLPAGES;
#endif

  for( iCntr = 20; iCntr > 0; iCntr-- )
    {
    if( HAL_OK == HAL_FLASHEx_OBProgram( &OBInit ) )
      break;

    HAL_Delay(1);
    
#if defined(STM32F427xx) | defined(STM32F429xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR));
#elif defined(STM32F407xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR));
#elif defined(STM32F103xE)
    /* Clear pending flags (if any) */  
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                           FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
#endif
    }
/*
  // Start the Option Bytes programming process.
  //if (FLASH_OB_Launch() != FLASH_COMPLETE)
  if (HAL_FLASH_OB_Launch() != HAL_OK)
    {
    HAL_FLASH_OB_Lock();
   
    // Error: Flash write unprotection failed
    return (1);
    }
*/
  HAL_FLASH_OB_Lock();

  /* Write Protection successfully disabled */
  return (0);
}

#if defined(STM32F427xx) | defined(STM32F429xx) | defined(STM32F407xx)
/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
static int GetSector(volatile uint32_t Address)
{
  
  int sector = 0;

  if( Address < FLASH_BASE )
    return -1;
  if( Address >= FLASH_BASE + SIZE_ChipFlash )
    return -1;

  if( Address < ADDR_FLASH_SECTOR_1 )
    sector = FLASH_SECTOR_0;
  else if( Address < ADDR_FLASH_SECTOR_2 ) 
    sector = FLASH_SECTOR_1;
  else if( Address < ADDR_FLASH_SECTOR_3 ) 
    sector = FLASH_SECTOR_2;
  else if( Address < ADDR_FLASH_SECTOR_4 ) 
    sector = FLASH_SECTOR_3;
  else if( Address < ADDR_FLASH_SECTOR_5 ) 
    sector = FLASH_SECTOR_4;
  else if( Address < ADDR_FLASH_SECTOR_6 ) 
    sector = FLASH_SECTOR_5;
  else if( Address < ADDR_FLASH_SECTOR_7 ) 
    sector = FLASH_SECTOR_6;
#ifdef ADDR_FLASH_SECTOR_8
  else if( Address <= ADDR_FLASH_SECTOR_8 ) 
    sector = FLASH_SECTOR_7;
  else if( Address <= ADDR_FLASH_SECTOR_9 ) 
    sector = FLASH_SECTOR_8;
  else if( Address <= ADDR_FLASH_SECTOR_10 ) 
    sector = FLASH_SECTOR_9;
  else if( Address <= ADDR_FLASH_SECTOR_11 ) 
    sector = FLASH_SECTOR_10;
  else if( Address <= CODE_FLASH_END_ADDRESS )
    sector = FLASH_SECTOR_11;
  else 
    sector = -1;
#else
  else if( Address <= CODE_FLASH_END_ADDRESS )
    sector = FLASH_SECTOR_7;
  else
    sector = -1;
#endif
  
  return sector;
}
#endif

/**
  * @brief  Disables the read protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_DisableReadProtection(void)
{
  
  FLASH_OBProgramInitTypeDef OBInit;
  int iCntr;

  HAL_FLASHEx_OBGetConfig( &OBInit );
  
  if( OB_RDP_LEVEL_0 != OBInit.RDPLevel )
    {
    // Unlock the Option Bytes
    //FLASH_OB_Unlock();
    HAL_FLASH_OB_Unlock();

    /* Disable the Read protection for all sectors inside the user flash area */
    //FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel   = OB_RDP_LEVEL_0;
    OBInit.Banks      = FLASH_BANK_1;
    //HAL_FLASHEx_OBProgram(&OBInit);
    for( iCntr = 20; iCntr > 0; iCntr-- )
      {
      if( HAL_OK == HAL_FLASHEx_OBProgram( &OBInit ) )
        break;

      HAL_Delay(1);
    
#if defined(STM32F427xx) | defined(STM32F429xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR));
#elif defined(STM32F407xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR));
#elif defined(STM32F103xE)
    /* Clear pending flags (if any) */  
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP   | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGERR | FLASH_FLAG_OPTVERR);
#endif
      }

    /* Start the Option Bytes programming process. */  
    //if (FLASH_OB_Launch() != FLASH_COMPLETE)
    if (HAL_FLASH_OB_Launch() != HAL_OK)
      {
      HAL_FLASH_OB_Lock();
     
      /* Error: Flash Read unprotection failed */
      return (1);
       }

    HAL_FLASH_OB_Lock();
    }

  /* Read Protection successfully disabled */
  return (0);
}

/**
  * @brief  Enables the read protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_EnableReadProtection(void)
{
  
  FLASH_OBProgramInitTypeDef OBInit;
  int iCntr;

  OBInit.RDPLevel = 0;
  HAL_FLASHEx_OBGetConfig( &OBInit );
  
  if( OB_RDP_LEVEL_1 != OBInit.RDPLevel )
    {
#if defined(STM32F427xx) | defined(STM32F429xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR));
#elif defined(STM32F407xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR));
#elif defined(STM32F103xE)
    /* Clear pending flags (if any) */  
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                           FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
#endif
    /* Unlock the Option Bytes */
    //FLASH_OB_Unlock();
    HAL_FLASH_OB_Unlock();

    /* Disable the write protection for all sectors inside the user flash area */
    //FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel   = OB_RDP_LEVEL_1;
    OBInit.Banks      = FLASH_BANK_1;
    //HAL_FLASHEx_OBProgram(&OBInit);
    for( iCntr = 20; iCntr > 0; iCntr-- )
      {
      if( HAL_OK == HAL_FLASHEx_OBProgram( &OBInit ) )
        break;

      HAL_Delay(1);
    
  #if defined(STM32F427xx) | defined(STM32F429xx)
      __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                              FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR));
  #elif defined(STM32F407xx)
    __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR));
  #elif defined(STM32F103xE)
      /* Clear pending flags (if any) */  
      __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|
                             FLASH_FLAG_PGERR|FLASH_FLAG_OPTVERR);
  #endif
      }
      
    HAL_Delay(10);

    /* Start the Option Bytes programming process. */  
    //if (FLASH_OB_Launch() != FLASH_COMPLETE)
    if (HAL_FLASH_OB_Launch() != HAL_OK)
      {
      HAL_FLASH_OB_Lock();
     
      /* Error: Flash write unprotection failed */
      return (1);
       }

    HAL_FLASH_OB_Lock();
      
    HAL_Delay(10);
      
    //HAL_NVIC_SystemReset();
    }

  /* Write Protection successfully disabled */
  return (0);
}
/**
  * @}
  */

uint32_t FLASH_If_GetReadProtection(void)
{

  FLASH_OBProgramInitTypeDef OBInit;

  OBInit.RDPLevel = 0;
  HAL_FLASHEx_OBGetConfig( &OBInit );
  
  return OBInit.RDPLevel;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
