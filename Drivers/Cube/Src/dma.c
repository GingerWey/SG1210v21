/**
  ******************************************************************************
  * File Name          : dma.c
  * Description        : This file provides code for the configuration
  *                      of all the requested memory to memory DMA transfers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "dma.h"

#include <stm32f4xx_hal.h>
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
//  __HAL_RCC_DMA1_CLK_ENABLE();

//           RX          TX
//  SPI1    DMA2_0_3  DMA2_3_3
//  UART1   DMA2_2_4  DMA2_7_4
  
  /* DMA2_Stream0_IRQn interrupt configuration */
  // SPI1_RX
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  // SPI1_TX
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ  (DMA2_Stream3_IRQn);

  // DMA2_Stream2_IRQn interrupt configuration
  // UART1_Rx
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 1);
  HAL_NVIC_EnableIRQ  (DMA2_Stream2_IRQn);  
  // DMA2_Stream7_IRQn interrupt configuration
  // UART1_Tx
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 1);
  HAL_NVIC_EnableIRQ  (DMA2_Stream7_IRQn);

  /* DMA2_Stream4_IRQn interrupt configuration */
  // ADC1
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
