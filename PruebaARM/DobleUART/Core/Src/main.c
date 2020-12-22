/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "H:\Jorge\UPM\master\TFM\PruebaARM\DobleUART\MDK-ARM\encode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "H:\Jorge\UPM\master\TFM\PruebaARM\DobleUART\MDK-ARM\decode.h"

//#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t a = 0x32;
size_t ReceiveBufferSize = 512;
uint8_t ifup[] = {0x00, 0x00, 0x10, 0x08, 0x18};
uint8_t ifdown[] = {0x00, 0x00, 0x10, 0x07, 0x17};
uint8_t ComClear[] = {0x00, 0x00, 0x10, 0x00, 0x10};
uint8_t Reset[] = {0x00, 0x00, 0x10, 0x03, 0x13};
uint8_t Status[] = {0x00, 0x00, 0x11, 0x05, 0x14}; //Look the answer in the KBI pdf
uint8_t ReadChannel[] = {0x00, 0x00, 0x11, 0x12, 0x03};
uint8_t WriteChannel[] = {0x00, 0x01, 0x10, 0x12, 0x08, 0x0B}; //Channel 15 predefined, change last uint8_t to change channell between 11-26
uint8_t ReadRole[] = {0x00, 0x00, 0x11, 0x19, 0x08};
uint8_t WriteRole[] = {0x00, 1, 16, 25, 11, 3}; //FED, look the table below and change the last uint8_t

/* //CKS for WriteRole
  0 = Not Configured
  1 = Router -- CKS = 09
  2 = REED (Router Elegible End Device) -- CKS = 0A
  3 = FED (Full End Device) -- CKS = OB
  4 = MED (Minimal End Device) -- CKS = 0C
  5 = SED (Sleepy End Device) CKS = 0D
  6 = Leader -- CKS = 0E

  Modifications conditions
  Status must be none, except none - saved configuration. Also allowed when status is joined to trigger some
  role transition mechanisms (SED ? MED, SED ? FED, MED? FED).
*/

uint8_t ReadJoinCred[] = {0x00, 0x00, 0x11, 0x17, 0x06};
uint8_t WriteJoinCred[] = {0x00, 0x10, 0x10, 0x17, 0x69, 0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x46, 0x43};

	

	
#define FRAME_HEADER_LEN 5
#define FRAME_PAYLOAD_MAX_LEN 1268


	
/* USER CODE END PV */
	
#define RX_SIZE 512
uint8_t PacketMarker = 0;
uint8_t cadena[3];
uint8_t receiveBuffer[RX_SIZE];
volatile int indice = 0;

/*int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)&ch, 1,100);
	return ch;
}*/
	
	
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

static void send(uint8_t *buffer, size_t size);
static void receive(void);
static void InicioFed(void);

static void hextobin( const char *str, uint8_t *dst, size_t len );

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	//HAL_UART_Receive_IT(&huart1, cadena, 1);

  /* USER CODE END 2 */
	InicioFed();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		HAL_UART_Transmit(&huart2, &a,1,10);
		HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
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
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
static void hextobin( const char *str, uint8_t *dst, size_t len )
{
  uint8_t pos, i0, i1;

  /* Mapping of ASCII characters to hex values */
  const uint8_t hashmap[] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
      0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
      0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
  };

  for ( pos = 0; ( ( pos < ( len * 2 ) ) && ( pos < strlen( str ) ) );
        pos += 2 )
  {
    i0             = ( ( uint8_t ) str[ pos + 0 ] & 0x1F ) ^ 0x10;
    i1             = ( ( uint8_t ) str[ pos + 1 ] & 0x1F ) ^ 0x10;
    dst[ pos / 2 ] = ( uint8_t )( hashmap[ i0 ] << 4 ) | hashmap[ i1 ];
  };
}

static  void InicioFed(void)
{
	//Clear
	HAL_Delay(1000);
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])));
	HAL_Delay(1000);
	
	
	//Channel	
	//printf("Channel\n\r");
	send(WriteChannel,(sizeof(WriteChannel)/sizeof(WriteChannel[0])));
	receive();
	
	
	//Role
	HAL_Delay(100);
	//printf("Role\n\r");
	send(WriteRole,(sizeof(WriteRole)/sizeof(WriteRole[0])));
	receive();
	//Join Credential
	
	send(WriteChannel,(sizeof(WriteJoinCred)/sizeof(WriteJoinCred[0])));
	receive();
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])));
	receive();
}

static void send(uint8_t *buffer, size_t size)
{
	uint8_t _encodeBuffer[getEncodedBufferSize(size)];
	size_t numEncoded = encode(buffer, size, _encodeBuffer);
	HAL_UART_Transmit(&huart1,&PacketMarker,sizeof(PacketMarker),1000);
	HAL_UART_Transmit(&huart1,_encodeBuffer,numEncoded,1000);
	
}

uint8_t uart_recvChar(uint8_t *byte) {
	return HAL_UART_Receive(&huart1, byte,1,1);
}

static void receive(void)
{
	int i=0;
	int16_t  result;
	uint8_t x= '\0';
	uint8_t decodedbuffer[FRAME_HEADER_LEN + FRAME_PAYLOAD_MAX_LEN];
	
	do
	{
		result = cobs_decode(decodedbuffer,1300, uart_recvChar);
		i++;
	}while(result == 0);
	
	HAL_UART_Transmit(&huart2, decodedbuffer,result,10);
	HAL_UART_Transmit(&huart2, &x,1,10);
	

}
/* USER CODE END 4 */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  //UNUSED(huart);
	if(huart->Instance == USART1)
	{
		uint8_t dato = cadena[0];
		receiveBuffer[indice++] = dato;
		if(indice >= RX_SIZE)
		{
			indice = 0;
		}
		if(indice >= RX_SIZE)
		{
			indice = 0;
		}
		
	}
	HAL_UART_Receive_IT(&huart1, cadena, 1);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
