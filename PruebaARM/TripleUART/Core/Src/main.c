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
#include "H:\Jorge\UPM\master\TFM\PruebaARM\TripleUART\MDK-ARM\encode.h"
//#include "H:\Jorge\UPM\master\TFM\PruebaARM\TripleUART\MDK-ARM\decode.h"
#include "H:\Jorge\UPM\master\TFM\PruebaARM\TripleUART\MDK-ARM\Comandos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t a = 0x32;
size_t ReceiveBufferSize = 512;
#define RX_SIZE 512
uint8_t cadena[3];
uint8_t receiveFed[RX_SIZE];
uint8_t receiveLeader[RX_SIZE];
volatile int indice1 = 0;
volatile int indice2 = 0;

/*int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)&ch, 1,100);
	return ch;
}*/
	
#define FRAME_HEADER_LEN 5
#define FRAME_PAYLOAD_MAX_LEN 1268


	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

static void send(uint8_t *buffer, size_t size, UART_HandleTypeDef modulo);
static void receive(UART_HandleTypeDef modulo);
static void InicioLeader(void);
static void InicioFed(void);
uint8_t *unite( uint8_t *buff1, uint8_t *buff2);
static uint8_t XOR_CKS(uint8_t *frame);
uint8_t uart_recvChar(uint8_t *byte);
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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

	//HAL_UART_Receive_IT(&huart1, cadena, 1);

  /* USER CODE END 2 */
	InicioLeader();
	InicioFed();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_Delay(1000);
	
	send(OpenSocket, (sizeof(OpenSocket)/sizeof(OpenSocket[0])),huart3);
	receive(huart3);
	HAL_Delay(200);
	send(OpenSocket, (sizeof(OpenSocket)/sizeof(OpenSocket[0])),huart1);
	receive(huart1);
	HAL_Delay(1000);
	
	SendHello[CKS_POS]=XOR_CKS(SendHello);
	
  while (1)
  {
    /* USER CODE END WHILE */
		send(Status,(sizeof(Status)/sizeof(Status[0])), huart1);
		receive(huart1);
		HAL_Delay(1000);
		send(SendHello,(sizeof(SendHello)/sizeof(SendHello[0])), huart1);
		receive(huart1);
		HAL_Delay(5000);
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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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

static uint8_t XOR_CKS(uint8_t *frame)
{
	uint8_t cks = 0;
	for(int i=0; i<((sizeof(frame))/(sizeof(frame[0])));i++)
	{
		if(i != CKS_POS)
			cks = cks^frame[i];
	}
	frame[CKS_POS] = cks;
	return cks;
}

static  void InicioFed(void)
{
	//Clear
	HAL_Delay(1000);
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])), huart1);
	HAL_Delay(3000);
	
	
	
	//Channel	
	//printf("Channel\n\r");
	send(WriteChannel,(sizeof(WriteChannel)/sizeof(WriteChannel[0])), huart1);
	receive(huart1);
	
	//Join Credential
	HAL_Delay(1000);
	send(WriteJoinCred,(sizeof(WriteJoinCred)/sizeof(WriteJoinCred[0])), huart1);
	receive(huart1);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");
  send(RoleFed,(sizeof(RoleFed)/sizeof(RoleFed[0])), huart1);
	receive(huart1);	
	

	HAL_Delay(1000);
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])), huart1);
	receive(huart1);
	HAL_Delay(10000);
	
	send(WriteIPFed, sizeof(WriteIPFed)/sizeof(WriteIPFed[0]),huart1);
	receive(huart1);
	HAL_Delay(1000);
}


static  void InicioLeader(void)
{
	//Clear
	HAL_Delay(1000);
	send(ComClear,(sizeof(ComClear)/sizeof(ComClear[0])), huart3);
	HAL_Delay(3000);
	
	
	
	//Channel	
	//printf("Channel\n\r");
	send(WriteChannel,(sizeof(WriteChannel)/sizeof(WriteChannel[0])), huart3);
	receive(huart3);
	
	//Join Credential
	HAL_Delay(1000);
	send(WriteJoinCred,(sizeof(WriteJoinCred)/sizeof(WriteJoinCred[0])), huart3);
	receive(huart3);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");
  send(RoleLeader,(sizeof(RoleLeader)/sizeof(RoleLeader[0])), huart3);
	receive(huart3);
		

	HAL_Delay(1000);
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])), huart3);
	receive(huart3);
	HAL_Delay(7000);
	
	send(WriteIPLeader, sizeof(WriteIPLeader)/sizeof(WriteIPLeader[0]),huart3);
	receive(huart3);
	HAL_Delay(1000);
	
	
	send(CommissionerOn, (sizeof(CommissionerOn)/sizeof(CommissionerOn[0])),huart3);
	receive(huart3);
	HAL_Delay(1000);
	
	
	
	send(AddJoiner, (sizeof(AddJoiner)/sizeof(AddJoiner[0])),huart3);
	receive(huart3);
	HAL_Delay(1000);
}


//Sending the command to the huart selected
static void send(uint8_t *buffer, size_t size, UART_HandleTypeDef modulo)
{
	uint8_t _encodeBuffer[getEncodedBufferSize(size)];
	size_t numEncoded = encode(buffer, size, _encodeBuffer);
	HAL_UART_Transmit(&modulo,&PacketMarker,sizeof(PacketMarker),1000);
	HAL_UART_Transmit(&modulo,_encodeBuffer,sizeof(_encodeBuffer)/sizeof(_encodeBuffer[0]),1000);
	
}

uint8_t uart_recvChar(uint8_t *byte) 
{
	return (*byte);
}



static void receive(UART_HandleTypeDef modulo)
{
	
	//HAL_Delay(10);	
	uint8_t receivebuffer[512];	
	uint8_t result;
	int i=0;
	uint8_t c;
	
	while(HAL_UART_Receive(&modulo, &c,1,1000) == HAL_OK)
	{
		receivebuffer[i]=c;
		i++;
	}
	uint8_t decodedbuffer[i*2 +1];
	
	/*do
	{
		result=cobs_decode(decodedbuffer,i*2 +1,uart_recvChar,(*receivebuffer)++);
		
	}while(result == 0);*/
	
	result=decode(receivebuffer,i*2 +1,decodedbuffer);
	HAL_UART_Transmit(&huart2, decodedbuffer,i*2 +1,10);
	//HAL_UART_Transmit(&huart2, &x,1,10);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  //UNUSED(huart);
	if(huart->Instance == USART1)
	{
		uint8_t datoA = cadena[0];
		receiveFed[indice1++] = datoA;
		if(indice1 >= RX_SIZE)
		{
			indice1 = 0;
		}
		if(indice1 >= RX_SIZE)
		{
			indice1 = 0;
		}
		HAL_UART_Receive_IT(&huart1, cadena, 1);
	}
	if(huart->Instance == USART3)
	{
		uint8_t datoB = cadena[0];
		receiveLeader[indice2++] = datoB;
		if(indice2 >= RX_SIZE)
		{
			indice2 = 0;
		}
		if(indice2 >= RX_SIZE)
		{
			indice2 = 0;
		}
		
		HAL_UART_Receive_IT(&huart3, cadena, 1);
	}  
}


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

