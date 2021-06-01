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
 uint8_t returned;


/* Encodd byte output function. */
typedef void (*cobs_byteOut_t)(uint8_t c);

/* Encodd byte input function. */
typedef char ( *cobs_byteIn_t )( uint8_t *c);
typedef char ( *cobs_byteIn_t2 )( uint8_t *c , uint8_t *m);
	

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
static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last);	   
void  *memset (void *dest, int val, size_t len); 


//#ifdef DEBUG_COBS
void debug_tx( uint8_t byte, _Bool first, _Bool last ) {debug( 1, byte, first,  last );}
void debug_rx( uint8_t byte, _Bool first, _Bool last ) {debug(0, byte, first,  last  );}
//#else
//#define debug_tx( uint8_t byte, _Bool first, _Bool last )( void ) 0
//#define debug_rx(  uint8_t byte, _Bool first, _Bool last)( void ) 0
//#endif


uint8_t uart_recvChar(uint8_t *byte) 
{	   	
	uint8_t leido = _getkey();
	byte[0] = leido;
	return sizeof(byte)/sizeof(byte[0]);
}
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
						cod = 1;
						cod_index = write_index++;
						read_index++;
				}
				else
				{
						encoddBuffer[write_index++] = buffer[read_index++];
						cod++;

						if (cod == 0xFF)
						{
								encoddBuffer[cod_index] = cod;
								cod = 1;
								cod_index = write_index++;
						}
				}
		}

		encoddBuffer[cod_index] = cod;

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
		printf("COBS_%sX: |", tx ? "T" : "R");

	if (!(first && last))
		printf(" %c ", byte);

	if (last)

		printf("|\n");
	else
		printf(":");
}


void  *memset (void *dest, int val, size_t len)
{
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}


