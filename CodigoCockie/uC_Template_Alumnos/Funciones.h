#include "COBS_Kirale.h"



static void send(uint8_t *buffer, size_t size);
static void receive();






//////////////////////////////

// SEND
// Sending the command to the huart selected
// Buffer --> The command to send, HEX packets
// Size --> The size of the packet

//////////////////////////////
static uint8_t XOR_CKS(uint8_t *frame, size_t size)
{
	uint8_t cks = 0;
	int i=0;
	while(i<size)
	{
		if(i != CKS_POS)
		{
			cks ^= frame[i];
		}
		else
		{
			cks=cks;
		}
		i++;
	}
	return cks;
	
}


static void send(uint8_t *buffer, size_t size)
{
	uint8_t _encodeBuffer[512];
	size_t numEncoded= 0;
	printf("\nEntrando al envio\n");
	buffer[CKS_POS]= XOR_CKS(buffer, size);
	printf("\nTerminado XOR\n");
	numEncoded = encod(buffer, size, _encodeBuffer);
	printf("%c",PacketMarker);
	printf(_encodeBuffer);
	//HAL_UART_Transmit(&PacketMarker,sizeof(PacketMarker),1000);
	//HAL_UART_Transmit(_encodeBuffer,sizeof(_encodeBuffer)/sizeof(_encodeBuffer[0]),1000);
		
}

//////////////////////////////

// RECEIVE
// Receiving the response from the Kirale Module via UART to the HOST


//////////////////////////////



static void receive()
{
	
	int16_t result; // The variable result will stored the num of bytes decoded by the cobs_decode function
	uint8_t decoddbuffer[512]; //The variable where the decoded buffer will be stored. If bigger responses are expected increment its size.
	do
	{
		//Receiving the Response and decoding byte to byte.
		//printf("leyendo un carater\n");
		result=cobs_decod(decoddbuffer,512,uart_recvChar);
	}while(result == 0);
	printf("\nImprimiendo Resultado\n");
	printf(decoddbuffer);


}

