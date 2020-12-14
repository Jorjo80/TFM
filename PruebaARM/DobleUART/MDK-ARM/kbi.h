#include "cmds.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
//#include <time.h>


/****************************************************************************
**                                                                         **
**                         DEFINITIONS AND MACROS                          **
**                                                                         **
****************************************************************************/

#define KBI_PORT_TOUT_MS 1000
#define KBI_CMD_RETRIES 3
#define KBI_MAX_SOCKETS 1

/****************************************************************************
**                                                                         **
**                         TYPEDEFS AND STRUCTURES                         **
**                                                                         **
****************************************************************************/

/* Socket handler function. */
typedef void ( *kbi_handler_t )( uint16_t locPort, uint16_t peerPort,
                                 char *peerName, uint8_t *udpPld,
                                 uint16_t udpPldLen );

/* Socket structure */
typedef struct kbi_socket_t
{
  uint16_t      locPort; /* If 0, not used */
  uint16_t      peerPort;
  char          peerName[ 32 ]; /* If empty, bind, else, connect */
  kbi_handler_t handler;
} kbi_socket_t;

/**
 * @file  kbi.c
 *
 * @brief KBI API functions.
 *
 */

 /****************************************************************************
 **                                                                         **
 **                              MODULES USED                               **
 **                                                                         **
 ****************************************************************************/


 /****************************************************************************
 **                                                                         **
 **                         TYPEDEFS AND STRUCTURES                         **
 **                                                                         **
 ****************************************************************************/

 /****************************************************************************
 **                                                                         **
 **                      PROTOTYPES OF LOCAL FUNCTIONS                      **
 **                                                                         **
 ****************************************************************************/

static kbi_socket_t *findSocket(uint16_t locPort);
void kbi_ntf( void );

/****************************************************************************
**                                                                         **
**                           EXPORTED VARIABLES                            **
**                                                                         **
****************************************************************************/

/****************************************************************************
**                                                                         **
**                            GLOBAL VARIABLES                             **
**                                                                         **
****************************************************************************/

/* List of device's sockets */
kbi_socket_t kbi_sockets[KBI_MAX_SOCKETS];

/****************************************************************************
**                                                                         **
**                           EXPORTED FUNCTIONS                            **
**                                                                         **
****************************************************************************/

_Bool kbi_init(UART_HandleTypeDef *huart1)
{
	uint8_t status;
	status = uart_init(huart1);
	if (status)
		memset(kbi_sockets, 0, sizeof(kbi_sockets));
	return status;
}

/***************************************************************************/
/***************************************************************************/
void kbi_finish(void) { uart_close(); }

/***************************************************************************/
/***************************************************************************/
_Bool kbi_cmd(uint8_t fc, uint8_t cmd, uint8_t *pld, uint16_t pldLen)
{
	uint8_t retries = KBI_CMD_RETRIES;
	while (retries)
	{
		cmds_send(CMDS_FTCMD | fc, cmd, pld, pldLen);
	/* Find matching response */
		if ((cmds_rx_buf.frame_s.typ & CMDS_FTRSP) &&
			(cmds_rx_buf.frame_s.cmd == cmd))
		{
			/* Processes that always have a minumum duration */
			switch (cmd)
			{
			case CMDS_CMD_CLEAR:
				HAL_Delay(1000);
				break;
			case CMDS_CMD_IFUP:
				HAL_Delay(5000);
				break;
			}
			return 1;
		}
	}
	retries--;
	return 0;
}
	

_Bool kbi_waitFor(uint8_t cmd, uint8_t *pld, uint16_t len, uint16_t tout)
{
	uint32_t end = tout;
	uint32_t time = 0;
	while (time < end)
	{
		if (kbi_cmd(CMDS_FCCMD_READ, cmd, NULL, 0))
		{
			if (cmds_rx_buf.frame_s.len >= len)
			{
				if (memcmp(pld, cmds_rx_buf.frame_s.pld, len) == 0)
					return 1;
			}
		}
		time++;
		HAL_Delay(1);
	}
	return 0;
}

