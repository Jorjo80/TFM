#include "COBS_Kirale.h"


static void send(uint8_t *buffer, size_t size);
static void receive();
uint8_t *output;





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
	int i = 0;
	size_t numEncoded= 0;
	buffer[CKS_POS]= XOR_CKS(buffer, size);
	numEncoded = encod(buffer, size, _encodeBuffer);

	printf("%c",PacketMarker);	
	for(i=0;i<numEncoded;i++)
	{
		 printf("%c",_encodeBuffer[i]);
	}
	 printf("\n");
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


static int decode(const uint8_t* encodedBuffer, int size, uint8_t* decodedBuffer)
{

        int read_index  = 0;
        int write_index = 0;
        uint8_t cod       = 0;
        uint8_t i          = 0;

        while (read_index < size)
        {
            cod = encodedBuffer[read_index];

            if (read_index + cod > size && cod != 1)
            {
                return 0;
            }

            read_index++;

            for (i = 1; i < cod; i++)
            {
                decodedBuffer[write_index++] = encodedBuffer[read_index++];
            }

            if (cod != 0xFF && read_index != size)
            {
                decodedBuffer[write_index++] = '\0';
            }
        }

        return write_index;
}

