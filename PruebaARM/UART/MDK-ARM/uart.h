
#include "main.h"
/****************************************************************************
**                                                                         **
**                              MODULES USED                               **
**                                                                         **
****************************************************************************/


#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>




bool uart_init(UART_HandleTypeDef huart1)
{
	huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
		return false;
  }
	return true;
}

/***************************************************************************/
/***************************************************************************/
void uart_sendChar(UART_HandleTypeDef huart1, uint8_t byte)
{
	HAL_UART_Transmit(&huart1,&byte, 1,1);
}

/***************************************************************************/
/***************************************************************************/
uint8_t uart_recvChar(UART_HandleTypeDef huart1, uint8_t byte) {
	return HAL_UART_Receive(&huart1,&byte, 1,1);
}

/***************************************************************************/
/***************************************************************************/
void uart_close(void)
{

}