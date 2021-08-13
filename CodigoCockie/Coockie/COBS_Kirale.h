/****************************************************************************
**                                                                         **
**                         DEFINITIONS AND MACROS                          **
**                                                                         **
****************************************************************************/

#include "comandos.h"


#define COBS_SIZE_codS_ARRAY 10

#define COBS_RESULT_NONE 0
#define COBS_RESULT_ERROR -1
#define COBS_RESULT_TIMEOUT -2

/****************************************************************************
**                                                                         **
**                         TYPEDEFS AND STRUCTURES                         **
**                                                                         **
****************************************************************************/

/* Encoded byte output function. */
typedef void (*cobs_byteOut_t)(uint8_t c);

/* Encoded byte input function. */
typedef char ( *cobs_byteIn_t )( uint8_t *c);
typedef char ( *cobs_byteIn_t2 )( uint8_t *c , uint8_t *m);

//Estructuras utilizadas por las funciones de la codificación COBS	

struct cobs_tx_s
{
	short pos[10];
	char  cod[10];
	char  codPos;
};

struct cobs_rx_s
{
	char dataBytes;
	char zeroes;
};

/* Struct of transmission using COBS. */
struct usart_tx_s
{
	short         totBytes; /* Total number of bytes to send. */
	short         proBytes; /* Number of processed bytes. */
	struct cobs_tx_s cobs;     /* COBS data. */
};

/* Struct of transmission using COBS. */
struct usart_rx_s
{
	short         totBytes; /* Total number of bytes to send. */
	short          proBytes; /* Number of bytes sent. */
	char          startMsg;
	char          payload;
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
Funcion Debug de los diferentes bytes que se transmiten o reciben

* _Bool tx --> Valor para diferenciar si es byte transmitido o recibido, Input
* uint8_t --> Byte que se envía o se recibe, Input
* _Bool first --> Valor para indicar el primer byte, Input
* _Bool last --> Valor para indicar el último byte, Input

*/
static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last);
	   
void  *memset (void *dest, int val, size_t len); 


//#ifdef DEBUG_COBS
void debug_tx( uint8_t byte, _Bool first, _Bool last ) {debug( 1, byte, first,  last );}
void debug_rx( uint8_t byte, _Bool first, _Bool last ) {debug(0, byte, first,  last  );}
//#else
//#define debug_tx( uint8_t byte, _Bool first, _Bool last )( void ) 0
//#define debug_rx(  uint8_t byte, _Bool first, _Bool last)( void ) 0
//#endif



/*
*Funcion para leer los bytes recibidos
* uint8_t byte --> Puntero a dirección de memoria donde se guardará el valor recicibdo y leido por puerto UART, IN/OUTPUT
*/
uint8_t uart_recvChar(uint8_t *byte) 
{	   	
	uint8_t leido = _getkey();
	byte[0] = leido;
	return sizeof(byte)/sizeof(byte[0]);
}

/*
* Función de Codificación COBS. Se utiliza función de librería "PacketSerial.h" para Arduino.

* const uint8_t* buffer --> Buffer que se desea codificar, Input
* size_t size --> Valor del tamaño del buffer, Input
* uint8_t* encoddBuffer	--> Buffer donde se guarda lo codificado, Output

*/
static size_t encod(const uint8_t* buffer, size_t size, uint8_t* encoddBuffer)
{
		size_t read_index  = 0;
		size_t write_index = 1;
		size_t cod_index  = 0;
		char cod       = 1;

		while (read_index < size)
		{
				if (buffer[read_index] == 0)
				{
						encoddBuffer[cod_index] = cod;
						//*encoddBuffer++ = cod;
						cod = 1;
						cod_index = write_index++;
						read_index++;
				}
				else
				{
						encoddBuffer[write_index++] = buffer[read_index++];
						/**encoddBuffer++ = *buffer++;
						write_index++;
						read_index++;*/	
						cod++;

						if (cod == 0xFF)
						{
								encoddBuffer[cod_index] = cod;
								//*encoddBuffer++ = cod;
								cod = 1;
								cod_index = write_index++;
						}
				}
		}

		encoddBuffer[cod_index] = cod;
		//*encoddBuffer++ = cod;
		return write_index;
}

// COBS KIRALE



/*int timeout(uint8_t inByte)
{
	debug_rx(inByte, 1, 1);
	return COBS_RESULT_TIMEOUT;
}

 int nothing()
{
	return COBS_RESULT_NONE;
}
int first(uint8_t inByte)
{
	debug_rx(inByte, 1, 0);
	return COBS_RESULT_NONE;
}
int error(uint8_t inByte)
 {
	debug_rx(inByte, 0, 1);
	return COBS_RESULT_ERROR;
}
int incomplete(uint8_t inByte)
{
	debug_rx(inByte, 0, 0);
	return COBS_RESULT_NONE;
}
int finished(uint8_t inByte)
{
	debug_rx(inByte, 0, 1);
	return (usart_rxPkt.totBytes);
}

*/

/* Función llamada por la codificación de Kirale, para enviar los datos por puerto UART
* uint8_t byte --> Valor a enviar
*/

void sendChar(uint8_t byte)
{
	printf("%c",byte);
}

/*
* Función para la codificación creada por KIRALE. Actualmente NO esta 100% implantada, no esta adptada al código utlizado en el resto de la Coockie
* uint8_t *buff --> Buffer a codificar
* uint16_t len --> Tamaño en bytes del Buffer
* cobs_byteOut_t output --> Función llamada para el envío de los bytes codificados. En este caso "sendChar"
*/
int16_t cobs_encod(uint8_t *buff, uint16_t len, cobs_byteOut_t output)
{
	struct usart_tx_s usart_txPkt;
	uint8_t *         tmpPtr = buff;
	uint8_t *         lastZeroPtr = NULL;
	uint8_t *         codPtr = NULL;
	uint16_t          codPos = 0;
	uint16_t          cod = 0x01;
	uint8_t           numZeroes = 0;
	int16_t           outIdx = 0;

	/* Initialize COBS structure. */
	memset(&usart_txPkt, 0, sizeof(struct usart_tx_s));
	codPtr = usart_txPkt.cobs.cod;

	/*
	 * proBytes = number of encodd bytes
	 * length   = Number of in data bytes to encod
	 */
	while (usart_txPkt.proBytes < len)
	{
		if (*tmpPtr == 0)
		{
			numZeroes++;
			if ((numZeroes == 2) && (cod != 0x01))
			{
				if ((cod + 0xDF) <= 0xFE)
				{
					/* The (n-E0) data bytes, plus two trailing zeroes. */
					cod += 0xDF;
					numZeroes = 0;
				}
				else
				{
					/* The (n-1) data bytes followed by a single zero. */
					numZeroes = 1;
				}

				*codPtr = cod;
				usart_txPkt.totBytes++;
				if (codPtr == usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos)
				{
					usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] = codPos;
					usart_txPkt.cobs.codPos++;
				}

				/* Reset counters. */
				codPtr = tmpPtr;
				codPos = usart_txPkt.proBytes;
				cod = 0x01;
			}
			else if (numZeroes == 0x0F)
			{
				/* We have reached maximun number of zeroes in a row. */
				cod = numZeroes + 0xD0;
				codPtr = lastZeroPtr;
				*codPtr = cod;
				usart_txPkt.totBytes++;

				/* Reset counters. */
				codPtr = tmpPtr;
				codPos = usart_txPkt.proBytes;
				numZeroes = 0;
				cod = 0x01;
			}
			else if ((codPtr ==
				usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos) &&
				(cod == 0x01) && (numZeroes == 1))
			{
				codPtr = tmpPtr;
				codPos = usart_txPkt.proBytes;
			}

			lastZeroPtr = tmpPtr;
		}
		else if (numZeroes)
		{
			/* Finish previous block. */
			if (numZeroes < 3)
			{
				if (cod == 0x01)
				{
					if (numZeroes == 2)
					{
						/* The (n-E0) data bytes, plus two trailing zeroes. */
						cod += 0xDF;
					}
					else
					{
						/* The (n-1) data bytes followed by a single zero. */
						/* Nothing to do. */
					}

					*codPtr = cod;
					usart_txPkt.totBytes++;

					if (codPtr != lastZeroPtr)
					{
						codPtr = lastZeroPtr;
						codPos = usart_txPkt.proBytes - 1;
					}
					else
					{
						/* Add aditional byte. */
						codPtr = usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos;
						codPos = usart_txPkt.proBytes;
					}
				}
				else
				{
					/* The (n-1) data bytes followed by a single zero. */
					*codPtr = cod;
					usart_txPkt.totBytes++;

					if (codPtr == usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos)
					{
						usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] = codPos;
						usart_txPkt.cobs.codPos++;
					}

					codPtr = lastZeroPtr;
					codPos = usart_txPkt.proBytes - 1;
				}
			}
			else
			{
				/* A run of (n-D0) zeroes. */
				cod = numZeroes + 0xD0;
				*codPtr = cod;
				usart_txPkt.totBytes++;
				codPtr = lastZeroPtr;
				codPos = usart_txPkt.proBytes - 1;
			}

			/* Reset counters. */
			numZeroes = 0;
			cod = 0x02;
			usart_txPkt.totBytes++;
		}
		else
		{
			/* Increment cod. */
			cod++;
			if (cod == 0xD0)
			{
				/*
				 * We have reached the maximum number of bytes not followed by a
				 * zero.
				 */
				*codPtr = 0xD0;
				usart_txPkt.totBytes++;
				if (codPtr == usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos)
				{
					usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] = codPos;
					usart_txPkt.cobs.codPos++;
				}

				/* Add aditional byte.*/
				codPtr = usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos;
				codPos = usart_txPkt.proBytes + 1;

				/* Reset counters. */
				cod = 0x01;
			}

			usart_txPkt.totBytes++;
		}

		/* Increment pointers. */
		tmpPtr++;
		usart_txPkt.proBytes++;
	}

	/* Finish message. */
	numZeroes++;
	if (numZeroes == 2)
	{
		if ((cod + 0xDF) <= 0xFE)
		{
			/* The (n-E0) data bytes, plus two trailing zeroes. */
			cod += 0xDF;
		}
		else
		{
			/* The (n-1) data bytes followed by a single zero. */
			*codPtr = cod;
			usart_txPkt.totBytes++;
			if (codPtr == usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos)
			{
				usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] = codPos;
				usart_txPkt.cobs.codPos++;
			}

			codPtr = lastZeroPtr;
			cod = 0x01;
		}
	}
	else if (numZeroes > 2)
	{
		/* A run of zeroes. */
		cod = numZeroes + 0xD0;
	}

	*codPtr = cod;
	usart_txPkt.totBytes++;
	if (codPtr == usart_txPkt.cobs.cod + usart_txPkt.cobs.codPos)
		usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] = codPos;

	/* Send out encodd bytes through the callback */
	usart_txPkt.proBytes = 0;
	usart_txPkt.cobs.codPos = 0;
	while (outIdx < usart_txPkt.totBytes + 1)
	{
		if (outIdx == 0)
		{
			/* First zero as start delimiter */
			debug_tx(0x00, 1, outIdx == usart_txPkt.totBytes);
			output(0x00);
			outIdx++;
		}
		else if ((usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] ==
			usart_txPkt.proBytes) &&
			(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos] != 0))
		{
			debug_tx(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos], 0, outIdx == usart_txPkt.totBytes);
			output(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos]);
			usart_txPkt.cobs.codPos++;
			outIdx++;
		}
		else
		{
			uint8_t dato = 0;

			while (dato == 0)
			{
				dato = buff[usart_txPkt.proBytes++];
				if (dato != 0)
				{
					debug_tx(dato, 0, outIdx == usart_txPkt.totBytes);
					output(dato);
					outIdx++;
				}
			}
		}
	}

	return (outIdx);
}


/*

* Función que implementa la decodificación de los carácteres que se reciben por puerto UART
* PARAMETROS
*	uint8_t *buff -->  Buffer donde se almacena la cadena decodificada
*   uint16_t len  -->  Longitud máxima que se espera decodificar.
*   uint8_t input -->  Es el byte a codificar
*
* Se retorna:
* 0 si aún no ha terminado de decodificar
* -1 si ha dado un error
* Número >0 cuando ha finalizado la decodificación indicando el número de bytes decodificados

*/


int16_t cobs_decod(uint8_t *buff, uint16_t len, char input)
{
	uint8_t inByte = 0;
	uint8_t numChar = 0;
	inByte = input;
	numChar = 1;
	if (numChar == 0)
	{
		//debug_rx(inByte, 1, 1);
		return COBS_RESULT_TIMEOUT;
	}
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
		{
			//debug_rx(inByte, 1, 0);
			return COBS_RESULT_NONE;
		}
		else
		{
			return COBS_RESULT_NONE;
		}
	}
	else if ((inByte != 0) && (usart_rxPkt.startMsg == 1))
	{
		if ((usart_rxPkt.proBytes >= 2) && (usart_rxPkt.payload == 0))
		{
			usart_rxPkt.totBytes += (buff[0] << 8) + buff[1];
			if (usart_rxPkt.totBytes > len)
			{
				//debug_rx(inByte, 0, 1);
				return COBS_RESULT_ERROR;
			}

			memset(buff + 5, 0, usart_rxPkt.totBytes - 5);
			usart_rxPkt.payload = 1;
		}

		if (usart_rxPkt.cobs.dataBytes == 0)
		{
			/* Read COBS cod. */
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
			{
				//debug_rx(inByte, 0, 1);
				return COBS_RESULT_ERROR;
			}
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
			{
				//debug_rx(inByte, 0, 1);
				return COBS_RESULT_ERROR;
			}

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
	{
		return COBS_RESULT_NONE;
	}

	if (usart_rxPkt.proBytes >= usart_rxPkt.totBytes)
	{
		usart_rxPkt.startMsg = 0;
		//debug_rx(inByte, 0, 0);
		return COBS_RESULT_NONE;
	}
	else
	{
		//debug_rx(inByte, 0, 1);
		return (usart_rxPkt.totBytes);
	}



}


 static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last)
{
	if (first)
		printf("");

	if (!(first && last))
		printf("");

	if (last)
		printf("");
	else
		;

}


void  *memset (void *dest, int val, size_t len)
{
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}


