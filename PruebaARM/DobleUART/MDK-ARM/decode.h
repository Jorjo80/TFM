#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
typedef void ( *cobs_byteOut_t )( uint8_t );

/* Encoded byte input function. */
typedef uint8_t ( *cobs_byteIn_t )( uint8_t * );

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

static void debug(_Bool tx, uint8_t byte, _Bool first, _Bool last);

#ifdef DEBUG_COBS
#define debug_tx( ... ) debug( 1, __VA_ARGS__ )
#define debug_rx( ... ) debug( 0, __VA_ARGS__ )
#else
#define debug_tx( ... ) ( void ) 0
#define debug_rx( ... ) ( void ) 0
#endif /* DEBUG_COBS*/

int16_t cobs_decode(uint8_t *buff, uint16_t len, cobs_byteIn_t input,uint8_t encoded)
{
	uint8_t inByte = 0;
	uint8_t numChar = 0;
	inByte = encoded;
	numChar = input(&inByte);
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

