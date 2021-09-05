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

#include "F:\Jorge\UPM\master\TFM\PruebaARM\TripleUART\MDK-ARM\COBS.h"
#include "F:\Jorge\UPM\master\TFM\PruebaARM\TripleUART\MDK-ARM\Comandos.h"



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

//Variables para los puertos UART a utilizar

UART_HandleTypeDef huart1; // Puerto UART dedicado para comunicación con un Dongle KTDG102

UART_HandleTypeDef huart2; // Puerto UART dedicado para comunicación con PC, debugueo de lo enviado a los Dongles y lo recibido de sus respuestas

UART_HandleTypeDef huart3; // Puerto UART dedicado para comunicación con un Dongle KTDG102

/* USER CODE BEGIN PV */



//Variable definida para el tamaño máximo de las respuestas esperadas de los Dongles
#define RX_SIZE 512


	

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */


/******* Cabecera Funciones Propias *******/
/*
* Se explican junto a la definición propia de la función
*/
/******************************************/

static void send(uint8_t *buffer, size_t size, UART_HandleTypeDef *modulo);
static void receive(UART_HandleTypeDef *modulo);
static void InicioReed(void);
static void InicioMed(void);
static uint8_t XOR_CKS(uint8_t *frame,size_t size);
uint8_t uart_recvChar(uint8_t *byte, UART_HandleTypeDef *modulo);



/// NET CONFIG VARIABLES ///

/*
* Estas variables se utilizan como payload de los comandos de configuración de red
* Se sacan para una fácil modificación más directa, sin entrar a fichero Comandos.h de estos parámetros
* Esta facilitación se debe al entorno de pruebas formado entre los Dongles y el módulo de STM, 
* Al estar en un entorno de pruebas se ha preferido tener las variables a configurar de manera más accesible
*/

uint8_t channel = 0x0B;
uint8_t PANID[] = {0x12, 0x34}; // PAN ID = 0x1234
uint8_t NetName[] = {0x4d, 0x79, 0x4e, 0x65, 0x74,0x77, 0x6f, 0x72, 0x6b};
uint8_t MeshLocPrefix[] = {0xfd, 0x12, 0x34, 0x00,0x38, 0x00, 0x00, 0x00, 0x00};
uint8_t MasterKey[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
uint8_t ExtPANID[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t ComCred[] = {0x4d, 0x79,0x50, 0x61, 0x73, 0x73,0x77, 0x6f, 0x72, 0x64,0x64};
uint8_t JoinCred[] = {0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x46, 0x43}; //Credencial 8404D200000005FC

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

  // Inicializmos y configuramos ambos nodos, uniéndolos a la red.
	InicioReed();
	HAL_Delay(2000);
  InicioMed();

	HAL_Delay(10000);
	
/* 
*  Una vez dentro de la red se procede a asignarles una dirección IP conocida por nosotros. Es opcional.
*  Introducir la asignación de la IP, facilita el saber las IP de los nodos a los que mandar los mensajes UDP posteriormente.
*  De otra manera se le asigna una dirección IP "desconocida" para el resto de nodos, ya que no existe comando KBI que pregunte por 
*  Las direcciones IP de los demás nodos.
*/
	send(WriteIPReed, sizeof(WriteIPReed)/sizeof(WriteIPReed[0]),&huart3);
	receive(&huart3);
	HAL_Delay(1000);
	send(WriteIP, sizeof(WriteIP)/sizeof(WriteIP[0]),&huart1);
	receive(&huart1);
	HAL_Delay(1000);

  // Abrimos socket en un puerto UDP de cada módulo Dongle
	
	send(OpenSocket2, (sizeof(OpenSocket2)/sizeof(OpenSocket2[0])),&huart3);
	receive(&huart3);
	HAL_Delay(1000);
	send(OpenSocket, (sizeof(OpenSocket)/sizeof(OpenSocket[0])),&huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	
	HAL_Delay(1000);
  
	// Generamos un bucle para enviar mensajes UDP desde ambos Dongles a los nodos formados por la plataforma de Coockies 
	// Dejando 1 segundo entre mensaje y mensaje y 5 segundos al final de la secuencia

	
  while (1)
  {
    // USER CODE END WHILE
		send(SendHello,(sizeof(SendHello)/sizeof(SendHello[0])), &huart1);
		HAL_Delay(1000);
		send(SendHello2,(sizeof(SendHello2)/sizeof(SendHello2[0])), &huart1);
		HAL_Delay(1000);
		send(SendHello3,(sizeof(SendHello3)/sizeof(SendHello3[0])), &huart1);
		HAL_Delay(1000);
		send(SendHello4,(sizeof(SendHello4)/sizeof(SendHello4[0])), &huart3);
		HAL_Delay(1000);
		send(SendHello5,(sizeof(SendHello5)/sizeof(SendHello5[0])), &huart3);
		HAL_Delay(1000);
		send(SendHello6,(sizeof(SendHello6)/sizeof(SendHello6[0])), &huart3);
		HAL_Delay(1000);
		//receive(&huart3);
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

/******** Funciones de unión a la red ********/

/*
*
* InicioMed. Se encarga de ejecutar el procedimiendo de unión a la red de uno de los Dongles con Rol MED con Commissioning Out-of-Band Activado.
* Se utilizan los parámetros de Red definidos al inicio.
* En todo proceso de unir a la red, se deberá empezar ejecutando el comando Clear, para asegurar que no haya ninguna configuración guardada
* Que pueda impedir el guardado de los nuevos valores. Tras ejecutar Clear deberán dejarse pasar 3 segundos para asegurar que se haya
* Ejecutado correctamente. Al finalizar la configuración se ejecuta el comando IfUp para unirse a la red, tras el cuál deberá
* Dejarse entre 5-10 segundos para asegurar que el proceso de unión a la red haya terminado.
*
*/

static  void InicioMed(void)
{
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])), &huart1);
	HAL_Delay(3000);
	
	send(OOB, sizeof(OOB)/sizeof(OOB[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");		
	size_t size1 = sizeof(WriteRole);
	size_t size2 = sizeof(med);
	uint8_t cmdRole[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdRole[i] = WriteRole[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdRole[i] = med;
	}
	//unite(WriteRole, &leader, cmdRole, size1, doble);
  send(cmdRole,(sizeof(cmdRole)/sizeof(cmdRole[0])), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	//Channel	
	//printf("Channel\n\r");
	
	size1 = sizeof(WriteChannel);
	size2 = sizeof(channel);
	uint8_t cmdChannel[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdChannel[i] = WriteChannel[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdChannel[i] = channel;
	}
	send(cmdChannel,(sizeof(cmdChannel)/sizeof(cmdChannel[0])), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	
	//WritePANID
	size1 = sizeof(WritePANID);
	size2 = sizeof(PANID);
	uint8_t cmdWPANID[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdWPANID[i] = WritePANID[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWPANID[i] = PANID[p];
	}	
	//unite(WritePANID, PANID, cmdWPANID);
	send(cmdWPANID, sizeof(cmdWPANID)/sizeof(cmdWPANID[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteNetName);
	size2 = sizeof(NetName);
	uint8_t cmdWNetName[size1+size2];
	//unite(WriteNetName, NetName, cmdWNetName);
	for(int i = 0; i<size1;i++)
	{
		cmdWNetName[i] = WriteNetName[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWNetName[i] = NetName[p];
	}	
	send(cmdWNetName, sizeof(cmdWNetName)/sizeof(cmdWNetName[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	

	size1 = sizeof(WriteMLocPref);
	size2 = sizeof(MeshLocPrefix);
	uint8_t cmdWMLP[size1 + size2];
	//unite(WriteMLocPref, MeshLocPrefix, cmdWMLP);
	for(int i = 0; i<size1;i++)
	{
		cmdWMLP[i] = WriteMLocPref[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWMLP[i] = MeshLocPrefix[p];
	}	

	send(cmdWMLP, sizeof(cmdWMLP)/sizeof(cmdWMLP[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteMK);
	size2 = sizeof(MasterKey);
	uint8_t cmdMK[size1 + size2];
	//unite(WriteMK, MasterKey, cmdMK);
	for(int i = 0; i<size1;i++)
	{
		cmdMK[i] = WriteMK[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdMK[i] = MasterKey[p];
	}	
	send(cmdMK, sizeof(cmdMK)/sizeof(cmdMK[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	////
	
	size1 = sizeof(WriteExtPID);
	size2 = sizeof(ExtPANID);
	uint8_t cmdExtPID[size1 + size2];
	//unite(WriteExtPID, ExtPANID, cmdExtPID);
	for(int i = 0; i<size1;i++)
	{
		cmdExtPID[i] = WriteExtPID[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdExtPID[i] = ExtPANID[p];
	}	
	
	send(cmdExtPID, sizeof(cmdExtPID)/sizeof(cmdExtPID[0]), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	//Join Credential

  size1 = sizeof(WriteComCred);
	size2 = sizeof(ComCred);
	send(WriteComCred,(sizeof(WriteComCred)/sizeof(WriteComCred[0])), &huart1);
	receive(&huart1);
	HAL_Delay(1000);
	
	
	
	size1 = sizeof(WriteJoinCred);
	size2 = sizeof(JoinCred);
	uint8_t cmdJoinCred[size1 + size2];
	//unite(WriteJoinCred, JoinCred, cmdJoinCred);
	for(int i = 0; i<size1;i++)
	{
		cmdJoinCred[i] = WriteJoinCred[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdJoinCred[i] = JoinCred[p];
	}	
	
	send(cmdJoinCred,(sizeof(cmdJoinCred)/sizeof(cmdJoinCred[0])), &huart1);
	receive(&huart1);	
	HAL_Delay(1000);
	
	send(WriteTxPower,(sizeof(WriteTxPower)/sizeof(WriteTxPower[0])), &huart1);
	receive(&huart1);	
	HAL_Delay(1000);
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])), &huart1);
	receive(&huart1);
	HAL_Delay(5000);
	
	//uint8_t *SetIP;
	//unite(WriteIP,IPFed, SetIP);
	
	
}


/*
*
* InicioMed. Se encarga de ejecutar el procedimiendo de unión a la red de uno de los Dongles con Rol REED con Commissioning Out-of-Band Activado.
* Se utilizan los parámetros de Red definidos al inicio.
* En todo proceso de unir a la red, se deberá empezar ejecutando el comando Clear, para asegurar que no haya ninguna configuración guardada
* Que pueda impedir el guardado de los nuevos valores. Tras ejecutar Clear deberán dejarse pasar 3 segundos para asegurar que se haya
* Ejecutado correctamente. Al finalizar la configuración se ejecuta el comando IfUp para unirse a la red, tras el cuál deberá
* Dejarse entre 5-10 segundos para asegurar que el proceso de unión a la red haya terminado.
*
*/

static  void InicioReed(void)
{
	//Clear
	send(ComClear, (sizeof(ComClear)/sizeof(ComClear[0])), &huart3);
	HAL_Delay(3000);
	
	send(OOB, sizeof(OOB)/sizeof(OOB[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	//Role
	HAL_Delay(1000);
	//printf("Role\n\r");		
	size_t size1 = sizeof(WriteRole);
	size_t size2 = sizeof(reed);
	uint8_t cmdRole[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdRole[i] = WriteRole[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdRole[i] = reed;
		;
	}
	//unite(WriteRole, &leader, cmdRole, size1, doble);
  send(cmdRole,(sizeof(cmdRole)/sizeof(cmdRole[0])), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	//Channel	
	//printf("Channel\n\r");
	
	size1 = sizeof(WriteChannel);
	size2 = sizeof(channel);
	uint8_t cmdChannel[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdChannel[i] = WriteChannel[i];
	}
	for(int i = size1; i<(size1+size2); i++)
	{
		cmdChannel[i] = channel;
	}
	send(cmdChannel,(sizeof(cmdChannel)/sizeof(cmdChannel[0])), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	
	//WritePANID
	size1 = sizeof(WritePANID);
	size2 = sizeof(PANID);
	uint8_t cmdWPANID[size1+size2];
	for(int i = 0; i<size1;i++)
	{
		cmdWPANID[i] = WritePANID[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWPANID[i] = PANID[p];
	}	
	//unite(WritePANID, PANID, cmdWPANID);
	send(cmdWPANID, sizeof(cmdWPANID)/sizeof(cmdWPANID[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteNetName);
	size2 = sizeof(NetName);
	uint8_t cmdWNetName[size1+size2];
	//unite(WriteNetName, NetName, cmdWNetName);
	for(int i = 0; i<size1;i++)
	{
		cmdWNetName[i] = WriteNetName[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWNetName[i] = NetName[p];
	}	
	send(cmdWNetName, sizeof(cmdWNetName)/sizeof(cmdWNetName[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	

	size1 = sizeof(WriteMLocPref);
	size2 = sizeof(MeshLocPrefix);
	uint8_t cmdWMLP[size1 + size2];
	//unite(WriteMLocPref, MeshLocPrefix, cmdWMLP);
	for(int i = 0; i<size1;i++)
	{
		cmdWMLP[i] = WriteMLocPref[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdWMLP[i] = MeshLocPrefix[p];
	}	

	send(cmdWMLP, sizeof(cmdWMLP)/sizeof(cmdWMLP[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	
	size1 = sizeof(WriteMK);
	size2 = sizeof(MasterKey);
	uint8_t cmdMK[size1 + size2];
	//unite(WriteMK, MasterKey, cmdMK);
	for(int i = 0; i<size1;i++)
	{
		cmdMK[i] = WriteMK[i];
	}
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdMK[i] = MasterKey[p];
	}	
	send(cmdMK, sizeof(cmdMK)/sizeof(cmdMK[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	////
	
	size1 = sizeof(WriteExtPID);
	size2 = sizeof(ExtPANID);
	uint8_t cmdExtPID[size1 + size2];
	//unite(WriteExtPID, ExtPANID, cmdExtPID);
	for(int i = 0; i<size1;i++)
	{
		cmdExtPID[i] = WriteExtPID[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdExtPID[i] = ExtPANID[p];
	}	
	
	send(cmdExtPID, sizeof(cmdExtPID)/sizeof(cmdExtPID[0]), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	//Join Credential

	/////
	
	
	size1 = sizeof(WriteJoinCred);
	size2 = sizeof(JoinCred);
	uint8_t cmdJoinCred[size1 + size2];
	//unite(WriteJoinCred, JoinCred, cmdJoinCred);
	for(int i = 0; i<size1;i++)
	{
		cmdJoinCred[i] = WriteJoinCred[i];
	}
	HAL_Delay(100);
	for(int i = size1, p=0; i<(size1+size2); i++, p++)
	{
		cmdJoinCred[i] = JoinCred[p];
	}	
	
	send(cmdJoinCred,(sizeof(cmdJoinCred)/sizeof(cmdJoinCred[0])), &huart3);
	receive(&huart3);	
	HAL_Delay(1000);
	
	send(WriteComCred,(sizeof(WriteComCred)/sizeof(WriteComCred[0])), &huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	
	//IFUP
	send(ifup,(sizeof(ifup)/sizeof(ifup[0])), &huart3);
	receive(&huart3);
	HAL_Delay(7000);
	
	
	
	/*send(CommissionerOn,sizeof(CommissionerOn)/sizeof(CommissionerOn[0]),&huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	send(AddBR,sizeof(AddBR)/sizeof(AddBR[0]),&huart3);
	receive(&huart3);
	HAL_Delay(1000);
	
	send(AddJoiner,sizeof(AddJoiner)/sizeof(AddJoiner[0]),&huart3);
	receive(&huart3);
	HAL_Delay(1000);*/
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNCIONES PARA RECEPCIÓN Y TRANSMISIÓN DE DATOS


//////////////////////////////

/*
*
* SEND
*
* Se calcula el valor del Checksum del comando, se codifica y se envía a través del puerto UART indicado.
* Previo al envío del comando codificado, se enviará un marcador de paquete (byte con valor 0x00)
* Este marcador sirve como indicador para el KTWM102 de que va a recibir un comando.
*
* Argumentos
* 
* uint8_t *buffer --> Comando a enviar
* size_t size --> Tamaño en bytes del comando.
* UART_HandleTypeDef *modulo --> Puerto UART a través del que enviar los datos.
*
*/

//////////////////////////////

static void send(uint8_t *buffer, size_t size, UART_HandleTypeDef *modulo)
{
	uint8_t _encodeBuffer[getEncodedBufferSize(size)];

	buffer[CKS_POS]=XOR_CKS(buffer, size); // Se calcula el Checksum con un XOR del resto de bytes del comando
	size_t numEncoded = encode(buffer, size, _encodeBuffer); // Se codifica el comando
	HAL_UART_Transmit(modulo,&PacketMarker,sizeof(PacketMarker),1000); // Se envia marcador de paquete
	HAL_UART_Transmit(modulo,_encodeBuffer,sizeof(_encodeBuffer)/sizeof(_encodeBuffer[0]),1000); //Envío del comando
}

//////////////////////////////

/* 
* RECEIVE
*
* Se gestiona la recepción de las respuestas del dongle correspondiente, desde su lectura hasta su decodificación
* Posteriormente a su decodificación se envía la respuesta decodificada al ordenador por puerto UART a través de huart2
* Pudiendo así comprobar los valores recibidos y verificar que todo sigue funcionando correctamente
*
* Argumentos
*
* UART_HandleTypeDef *modulo --> Puerto UART del que se espera recibir datos.
* 
*/
static void receive(UART_HandleTypeDef *modulo)
{
	
	int16_t result; // La variable result guardara el número de bytes que son decodificados por la función cobs_decode
	uint8_t decodedbuffer[RX_SIZE]; // Buffer donde se almacenarán los bytes decodificados.
	do
	{
		// Recibimos y decodificamos la respuesta del módulo KTWM102 byte a byte y se deco.
		result=cobs_decode(decodedbuffer,RX_SIZE,uart_recvChar, modulo);
	}while(result == 0);
	
	HAL_UART_Transmit(&huart2, decodedbuffer,result,10);

}

/* 
* uart_recvChar
*
* Función llamada por cobs_decode para la lectura de los bytes recibidos en el puerto UART indicado.
* Adaptación de la creada por Kirale al entorno de trabajo y funciónes de lectura utilizadas
* 
* Argumentos
*
* uint8_t *byte --> puntero a lugar de memoria donde almacenar el dato recibido
* UART_HandleTypeDef *modulo --> Puerto UART del que se espera recibir datos
* 
* Se retorna el número de bytes leidos (Siempre 1)
* 
*/

uint8_t uart_recvChar(uint8_t *byte, UART_HandleTypeDef *modulo) 
{
	
	HAL_UART_Receive(modulo, byte,1,1000);
	uint8_t returned = sizeof(byte)/sizeof(byte[0]);
	return returned;
}




/* 
* XOR_CKS
*
* Función llamada por SEND
* 
* Argumentos
*
* uint8_t *frame --> Comando del que calcular el Checksum
* size_t size --> Tamaño en bytes del comando
* 
* Se retorna el valor checksum calculado.
* 
*/

static uint8_t XOR_CKS(uint8_t *frame, size_t size)
{
	uint8_t cks = 0;
	for(int i=0; i<size;i++)
	{
		if(i != CKS_POS)
			cks ^= frame[i];
		else
			cks=cks;
	}
	return cks;
	
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

