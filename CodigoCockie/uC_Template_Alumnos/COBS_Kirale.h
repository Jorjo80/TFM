
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"

/****************************************************************************
**                                                                         **
**                         DEFINITIONS AND MACROS                          **
**                                                                         **
****************************************************************************/

#define COBS_SIZE_codS_ARRAY 10

#define COBS_RESULT_NONE 0
#define COBS_RESULT_ERROR -1
#define COBS_RESULT_TIMEOUT -2

/****************************************************************************
**                                                                         **
**                         TYPEDEFS AND STRUCTURES                         **
**                                                                         **
****************************************************************************/

/* Encodd byte output function. */
typedef void ( *cobs_byteOut_t )(char c);

/* Encodd byte input function. */
typedef char ( *cobs_byteIn_t )( char *c);
typedef char ( *cobs_byteIn_t2 )( char *c , char *m);
	

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

static void debug(_Bool tx, char byte, Bool first, _Bool last);

#ifdef DEBUG_COBS
#define debug_tx( ... ) debug( 1, __VA_ARGS__ )
#define debug_rx( ... ) debug( 0, __VA_ARGS__ )
#else
#define debug_tx( ... ) ( void ) 0
#define debug_rx( ... ) ( void ) 0
#endif /* DEBUG_COBS*/

// COBS ENcod PROPIO
static size_t getEncoddBufferSize(size_t unencoddBufferSize)
{
		return unencoddBufferSize + unencoddBufferSize / 254 + 1;
}

static size_t encod(const char* buffer, size_t size, char* encoddBuffer)
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

int16_t cobs_encod(char *buff, short len, cobs_byteOut_t output, UART_HandleTypeDef *modulo)
{
	struct usart_tx_s usart_txPkt;
	char *         tmpPtr = buff;
	char *         lastZeroPtr = NULL;
	char *         codPtr = NULL;
	short          codPos = 0;
	short          cod = 0x01;
	char           numZeroes = 0;
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
			output(0x00, modulo);
			outIdx++;
		}
		else if ((usart_txPkt.cobs.pos[usart_txPkt.cobs.codPos] ==
			usart_txPkt.proBytes) &&
			(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos] != 0))
		{
			debug_tx(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos], 0,
				outIdx == usart_txPkt.totBytes);
			output(usart_txPkt.cobs.cod[usart_txPkt.cobs.codPos], modulo);
			usart_txPkt.cobs.codPos++;
			outIdx++;
		}
		else
		{
			char data = 0;

			while (data == 0)
			{
				data = buff[usart_txPkt.proBytes++];
				if (data != 0)
				{
					debug_tx(data, 0, outIdx == usart_txPkt.totBytes);
					output(data,modulo);
					outIdx++;
				}
			}
		}
	}

	return (outIdx);
}





int16_t cobs_decod(char *buff, short len, cobs_byteIn_t input, UART_HandleTypeDef *modulo)
{
	char inByte = 0;
	char numChar = 0;
	numChar = input(&inByte, modulo);
	if (numChar == 0)
		goto timeout;

	if (inByte == 0)
	{
		short dataBytes = usart_rxPkt.cobs.dataBytes;
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

/****************************************************************************
**                                                                         **
**                             LOCAL FUNCTIONS                             **
**                                                                         **
****************************************************************************/


/**
 * @brief Decod a UART message byte per byte.
 *
 * @param[out]     buff:  Pointer to the decodd message.
 * @param[in]      len:   Length limit of buff.
 * @param[in]      input: Pointer to encodd byte input callback (UART rx).
 *
 * @return          0: Decod not finished.
 *                 -1: Decod error.
 *                 -2: Port timeout.
 *                 >0: Length of the decodd message.
 */



int16_t cobs_decodInt(char *buff, short len, cobs_byteIn_t2 input, char *input_buff)
{
	char inByte = 0;
	char numChar = 0;
	numChar = input(&inByte, input_buff++);
	if (numChar == 0)
		goto timeout;

	if (inByte == 0)
	{
		short dataBytes = usart_rxPkt.cobs.dataBytes;
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




static void debug(_Bool tx, char byte, _Bool first, _Bool last)
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

