//En este documento están declaradas las estructuras y funciones necesarias para codificación y decodificación COBS


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include <inttypes.h>

/****************************************************************************
**                                                                         **
**                         DEFINITIONS AND MACROS                          **
**                                                                         **
****************************************************************************/

#define COBS_SIZE_CODES_ARRAY 10

#define COBS_RESULT_NONE 0
#define COBS_RESULT_ERROR -1
#define COBS_RESULT_TIMEOUT -2

/****************************************************************************
**                                                                         **
**                         TYPEDEFS AND STRUCTURES                         **
**                                                                         **
****************************************************************************/

/* Encoded byte output function. */
typedef void ( *cobs_byteOut_t )( uint8_t c, UART_HandleTypeDef *m );

/* Encoded byte input function. */
typedef uint8_t ( *cobs_byteIn_t )( uint8_t *c , UART_HandleTypeDef *m);
typedef uint8_t ( *cobs_byteIn_t2 )( uint8_t *c , uint8_t *m);
	

struct cobs_tx_s
{
	uint16_t pos[COBS_SIZE_CODES_ARRAY];
	uint8_t  code[COBS_SIZE_CODES_ARRAY];
	uint8_t  codePos;
};

struct cobs_rx_s
{
	uint8_t dataBytes;
	uint8_t zeroes;
};

/* Struct of transmission using COBS. */
struct usart_tx_s
{
	uint16_t         totBytes; /* Total number of bytes to send. */
	uint16_t         proBytes; /* Number of processed bytes. */
	struct cobs_tx_s cobs;     /* COBS data. */
};

/* Struct of transmission using COBS. */
struct usart_rx_s
{
	uint16_t         totBytes; /* Total number of bytes to send. */
	int16_t          proBytes; /* Number of bytes sent. */
	uint8_t          startMsg;
	uint8_t          payload;
	struct cobs_rx_s cobs; /* COBS data. */
};

/****************************************************************************
**                                                                         **
**                            GLOBAL VARIABLES                             **
**                                                                         **
****************************************************************************/

static struct usart_rx_s usart_rxPkt;

/****************************************************************************
**                                                                         **
**                      PROTOTYPES OF LOCAL FUNCTIONS                      **
**                                                                         **
****************************************************************************/

/*

* Funcion Debug de los diferentes bytes que se transmiten o reciben.
* Se muestran por pantalla a la vez que se transmiten o se reciben.
* 
* _Bool tx --> Valor para diferenciar si es byte transmitido o recibido, Input
* uint8_t --> Byte que se envía o se recibe, Input
* _Bool first --> Valor para indicar el primer byte, Input
* _Bool last --> Valor para indicar el último byte, Input
*
*/



static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last);

#ifdef DEBUG_COBS
#define debug_tx( ... ) debug( 1, __VA_ARGS__ )
#define debug_rx( ... ) debug( 0, __VA_ARGS__ )
#else
#define debug_tx( ... ) ( void ) 0
#define debug_rx( ... ) ( void ) 0
#endif /* DEBUG_COBS*/

/******************* COBS ENCODE PROPIO *******************/

/*

* Funcion getEncodedBufferSize. Se obtiene de libreria PacketSerial.h (Arduino)
*
* Se calcula el tamaño del buffer donde se almacenará el comando codificado
* 
* Variable de entrada
* size_t unencodedBufferSize --> Tamaño en bytes del comando a codificar
* 
* Se retorna el tamaño del buffer donde se almacenará el comando codificado
*
*/

static size_t getEncodedBufferSize(size_t unencodedBufferSize)
{
		return unencodedBufferSize + unencodedBufferSize / 254 + 1;
}


/*

* Funcion encode. Se obtiene de libreria PacketSerial.h (Arduino)
*
* Se codifica el comando deseado
* 
* Variable de entrada
* uint8_t* buffer --> Comando a codificar
* size_t size --> Tamaño del comando a codificar
* uint8_t* encodedBuffer --> Buffer donde se almacenará el comando codificado
* 
* Se retorna el número de bytes codificados
*
*/

static size_t encode(const uint8_t* buffer, size_t size, uint8_t* encodedBuffer)
{
		size_t read_index  = 0;
		size_t write_index = 1;
		size_t code_index  = 0;
		uint8_t code       = 1;

		while (read_index < size)
		{
				if (buffer[read_index] == 0)
				{
						encodedBuffer[code_index] = code;
						code = 1;
						code_index = write_index++;
						read_index++;
				}
				else
				{
						encodedBuffer[write_index++] = buffer[read_index++];
						code++;

						if (code == 0xFF)
						{
								encodedBuffer[code_index] = code;
								code = 1;
								code_index = write_index++;
						}
				}
		}

		encodedBuffer[code_index] = code;

		return write_index;
}


/******************* COBS DECODE KIRALE *******************/

/*
* Función cobs_decode
*
* Función creada por KIRALE para la decodificación
* 
* Ha sido adaptada para las comunicaciones UART del módulo utilizado.
*
* Decodificación en tiempo real de las respuestas del módulo KTWM102. Decodifica byte a byte según se van recibiendo
* 
* PARAMETROS
*		uint8_t *buff -->  Buffer donde se almacena la cadena decodificada
* 	uint16_t len  -->  Longitud máxima que se espera decodificar.
* 	cobs_byteIn_t input -->  Función a la que se llamará para la lectura de los bytes entrantes
*   UART_HandleTypeDef *modulo --> Entrada UART de la que leer los bytes.
*
* Se retorna:
* 	0 si aún no ha terminado de decodificar
* 	-1 si ha dado un error
* 	Número >0 cuando ha finalizado la decodificación indicando el número de bytes decodificados
*
*/



int16_t cobs_decode(uint8_t *buff, uint16_t len, cobs_byteIn_t input, UART_HandleTypeDef *modulo)
{
	uint8_t inByte = 0;
	uint8_t numChar = 0;
	numChar = input(&inByte, modulo);
	if (numChar == 0)
		goto timeout;

	if (inByte == 0)
	{
		uint16_t dataBytes = usart_rxPkt.cobs.dataBytes;
		/* Initialize COBS structure. */
		usart_rxPkt.totBytes = 5;
		usart_rxPkt.proBytes = 0;
		usart_rxPkt.startMsg = 1;
		usart_rxPkt.payload = 0;
		usart_rxPkt.cobs.dataBytes = 0;
		usart_rxPkt.cobs.zeroes = 0;
		memset(buff, 0, 5);
		if (dataBytes == 0)
			goto first;
		else
			goto nothing;
	}
	else if ((inByte != 0) && (usart_rxPkt.startMsg == 1))
	{
		if ((usart_rxPkt.proBytes >= 2) && (usart_rxPkt.payload == 0))
		{
			usart_rxPkt.totBytes += (buff[0] << 8) + buff[1];
			if (usart_rxPkt.totBytes > len)
				goto error;

			memset(buff + 5, 0, usart_rxPkt.totBytes - 5);
			usart_rxPkt.payload = 1;
		}

		if (usart_rxPkt.cobs.dataBytes == 0)
		{
			/* Read COBS code. */
			if (inByte < 0xD0)
			{
				usart_rxPkt.cobs.dataBytes = inByte - 1;
				usart_rxPkt.cobs.zeroes = 1;
			}
			else if (inByte == 0xD0)
			{
				usart_rxPkt.cobs.dataBytes = inByte - 1;
				usart_rxPkt.cobs.zeroes = 0;
			}
			else if ((inByte == 0xD1) || (inByte == 0xD2))
				goto error;
			else if (inByte < 0xE0)
			{
				/* Move pointer to the new position. */
				usart_rxPkt.cobs.dataBytes = 0;
				usart_rxPkt.cobs.zeroes = inByte - 0xD0;
			}
			else if (inByte < 0xFF)
			{
				usart_rxPkt.cobs.dataBytes = inByte - 0xE0;
				usart_rxPkt.cobs.zeroes = 2;
			}
			else
				goto error;

			if (usart_rxPkt.cobs.dataBytes == 0)
			{
				usart_rxPkt.proBytes += usart_rxPkt.cobs.zeroes;
				usart_rxPkt.cobs.zeroes = 0;
			}
		}
		else
		{
			if (usart_rxPkt.proBytes < usart_rxPkt.totBytes)
			{
				/* Read data byte. */
				buff[usart_rxPkt.proBytes] = inByte;
			}

			++usart_rxPkt.proBytes;
			if (--usart_rxPkt.cobs.dataBytes == 0)
			{
				usart_rxPkt.proBytes += usart_rxPkt.cobs.zeroes;
				usart_rxPkt.cobs.zeroes = 0;
			}
		}
	}
	else
		goto nothing;

	if (usart_rxPkt.proBytes >= usart_rxPkt.totBytes)
	{
		usart_rxPkt.startMsg = 0;
		goto finished;
	}
	else
		goto incomplete;

	
/*
*	ETIQUETAS
*	
* Utilizadas en cobs_decode para debuguear los bytes recibidos
* Según el caso donde se entre, se finalizará la función de decodificar, se devolverá error o se continuará decodificando	
*	
*/	
	
timeout:
	debug_rx(inByte, 1, 1);
	return COBS_RESULT_TIMEOUT;
nothing:
	return COBS_RESULT_NONE;
first:
	debug_rx(inByte, 1, 0);
	return COBS_RESULT_NONE;
error:
	debug_rx(inByte, 0, 1);
	return COBS_RESULT_ERROR;
incomplete:
	debug_rx(inByte, 0, 0);
	return COBS_RESULT_NONE;
finished:
	debug_rx(inByte, 0, 1);
	return (usart_rxPkt.totBytes);
}


static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last)
{
	if (first)
		printf("COBS_%sX: |", tx ? "T" : "R");

	if (!(first && last))
		printf(" %02x ", byte);

	if (last)
		printf("|\n");
	else
		printf(":");
}

