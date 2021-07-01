#include <ADuC841.h>
#include <stdio.h>

#include "COBS_Kirale.h"



/**************** FPGA Communication Port: ********************/
sbit TrigByte1N  = P0^7;		   
sbit Ack1N       = P0^6;          
sbit TrigByte2N  = P0^5;
sbit Ack2N       = P0^4;
sbit SelTrigger	 = P0^3;
sbit reset_fpga  = P0^0;
/**************************************************************/

unsigned char DATA_L;
unsigned char DATA_H;
unsigned int sizeRpl;
unsigned int i, p;


uint8_t cadena[256];
uint8_t decoded[256];
uint8_t dato;
unsigned char flag, c;

unsigned int result,Temp,Hum, LDR, cuenta_temp, cuenta_hum; 

unsigned int resulti[2];

/***************** Timer Configuration: **************************/
void _WS_Timer_Config(char value)
{
   	IEIP2 	= 0xA4; // TIC Interruption enable
	SEC 	= 0x00;
	HTHSEC	= 0x00;
	MIN		= 0x00;
	HOUR	= 0x00;
	INTVAL	= value;	//**(Config.)	
	TIMECON	= 0x53; 	// The timer interrupt each second **(Config.)	/* 0x43 = 1/128 seconds */
}	
/*****************************************************************/

/***************** ADC Configuration: ****************************/
void _WS_ADC_Config (void)
{
  	ADCCON1  = 0xAC;     // ADCCON1: ADC Configuration: 12 clock periods for each conversion.                                                                     
	ADCCON2  = 0x03;     // Selects channel 3 & on demand conversion.  (LDR is connected to the ADC3)
}
/*****************************************************************/

/***************** UART configuration: ***************************/
void _WSN_UART841_config()
{
	SCON = 0x52;//SCON: UART Serial Port Control Register	=> Mode 1: 8-bit UART, variable baud rate
	PCON = 0x80;//PCON: power-saving options and general-purpose status flags => SMOD=1 (Double UART Baud Rate)
	
	TMOD = 0x21;//Timer 1 Set M1 for 8-bit autoreload timer, Timer 0 Set M0 16-bit 
	TH1 =  0xFA; // 115200 ADuC841        //TH1 holds a value which is to be reloaded into TL1 each time it overflows. (BaudRate = 19200 bps)
	TR1  = 1;   //Start timer 1

	TI  = 1;   //bit1(SCON): Serial Port Transmit Interrupt Flag.
	ES  = 0;	// Serial Port interruption disable
	ET1 = 0;	// Timer 1 Interruption Disable	

	EA  = 1; 	// Global Enable Interruption Flag
}
/****************************************************************/

/****************** ADC Conversion: *****************************/
int _WSN_ADC_conversion()
{
	unsigned int sensorData;

	//*** Sigle conversion:
	SCONV = 1;
	while (SCONV == 1);

	sensorData = ((ADCDATAH & 0x0F) * 0x0100) + ADCDATAL;
	
	SCONV = 0; // Conversion Flag
	
	return (sensorData);

}
/*****************************************************************/

/****************** FPGA Initial config. *************************/
void _WSN_ini_FPGA(void)
{ 
 	   TrigByte1N  = 1;		   
	   TrigByte2N  = 1;
	   SelTrigger  = 0;
	   reset_fpga  = 1;
}
/****************** FPGA-DATA capture: **************************/
int _WSN_FPGA(bit sensorSelector)
{     
   unsigned int fpga_data;
         
   SelTrigger  = sensorSelector;
   TrigByte1N = 0; 
   while (Ack1N == 1){};
   
   DATA_L = P2;    	   // LSB
   TrigByte1N = 1;	   // Release Trigger1
    
   TrigByte2N = 0;	   //Trigger second data byte
   while (Ack2N == 1){};
   
   DATA_H = P2; 	  // MSB
   TrigByte2N = 1;    // Release Trigger2


   fpga_data = DATA_L + 256*(int)DATA_H;

   return(fpga_data);  
   
}
/*****************************************************************/
																	
/***************** Timer Interruption: ***************************/
void _WSN_interrupt_TimeInterval() interrupt 10 using 3 
{ 
   //unsigned int result,Temp,Hum,LDR,Axis;

      /** DO NOT EDIT *********/
	   c++;
	   if (c==2){
	     reset_fpga = 0;
		 reset_fpga = 1;
		 c = 0;
	   }   
	  /************************/
   
   
   flag = 1;

      
}
/*****************************************************************/

/***************** Sensors reading functionalities: ***************/
void _WSN_sensors_reading(void){

   int result[2], i;
   int estado;
   static int cuenta = 0;
   cuenta++;

   	/** _WSN_FPGA(0) = Temperature, ACC Y **/
	if(cuenta%cuenta_temp == 0)
	{
		result[0] = _WSN_FPGA(0); 
		Temp = (result[0] - (273.15*100)); 
		printf("La temperatura son %.2f grados celsius\n", ((float)Temp)/100);
		if(Temp>=25)
			estado = 1;
		else if(Temp<25)
			estado = 2;
	}

	for(i=0;i<100;i++);

	/** _WSN_FPGA(1) = Humidity, ACC X **/ 
	if(cuenta%cuenta_hum == 0)
	{
		result[1] = _WSN_FPGA(1);  
    	Hum =  ( (result[1]*127.0)/100 );  
		printf("Hay una humedad del %.2f %c\n", ((float)Hum)/100,37);	
	}


   /************ Temp: ******************/
   // the temperature value taken from the FPGA has	to be
   // substracted from 27315 in order to show Degree Celsius x 100				
   // Ej: Temp =  ( result - (273.15*100) );	  // Degree Celsius x 100
   // c = 0;
   /*************************************/
	

   /************ Humidity ***************/
   // the humidity value taken form the FPGA has to be multipled
   // by 127.5 and divided by 100 in order to show H% x 100.
   // Ej: Hum =  ( (result*127.0)/100 );
   // c = 0;
   /*************************************/


   /************ Light: ******************/

   /**************************************/	


}
/*****************************************************************/

/****************** ZigBee read: *********************************/
/** ASCII  = Value of the character to wait.
/** getsmj = It allows to get caracters from the serial port and 
/** print them until ASCII arrives. 
**/

void _WSN_wait_answer(char ASCII,char getmsj)
{  
	unsigned char serial_read,enable;

	enable = 1;
  
	     do
		{
			serial_read = _getkey(); 
	
			if (serial_read == ASCII) 
			{											 
			 	enable = 0;
			}
			else if (getmsj == 1)
			{
				putchar(serial_read);
			}			
		}while (enable != 0);
}
/**************** ZigBee Configuration: ************************/
void _WSN_ZigBee_config(char type)
{ 	
	printf("ATS00=0004\r");
	_WSN_wait_answer('O',0);
	_WSN_wait_answer('K',0);

	printf("ATS02=0003\r");
	_WSN_wait_answer('O',0);
	_WSN_wait_answer('K',0);

	printf("ATS03=0000000000000003\r");
	_WSN_wait_answer('O',0);
	_WSN_wait_answer('K',0);

	printf("AT+JN\r");
	_WSN_wait_answer('O',0);
	_WSN_wait_answer('K',0);
	
}
/******************* Message Detection: *************************/
void _WSN_message_detect()
{  
 	_WSN_wait_answer('U',0);
	_WSN_wait_answer(':',0);
	_WSN_wait_answer(',',1);
	 putchar('\t');
	_WSN_wait_answer('=',0);
	_WSN_wait_answer(0x03,1); 
}

/*********** FUNCIONES PROPIAS***************/


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


static void send(char *buffer, size_t size)
{
	char _encodeBuffer[512];
	int d = 0;
	size_t numEncoded= 0;
	buffer[4]= XOR_CKS(buffer, size);
	numEncoded = encod(buffer, size, _encodeBuffer);
	printf("%c",PacketMarker);	
	for(d=0;d<numEncoded;d++)
	{
		 printf("%c",_encodeBuffer[d]);
	}
		
}


static void receive(int len)
{
	int payload;
	while(i<len)
	{
		cadena[i]= _getkey();
		i++;
	}
	for(p=1;p<len;p++)
		printf("%c",cadena[p]);
	printf("\n");
	for(p=0;p<len;p++)
	{
		cobs_decod(decoded, len, cadena[p]);
	}
	payload= (int) decoded[1];

	for(p=0;p<(5+payload);p++)
	{
		if(decoded[p]==0x00)
			printf("%c", decoded[p]+0x0E);
		else
			printf("%c",decoded[p]);
	}
}

void delay()
{
	if(flag == 1)
	{
		flag = 0;
	}
	while(flag == 0)
	{
		;
	}
}
void InicioRed(char role)
{
   	send(OOB, (sizeof(OOB)));
	receive(6);
	i = 0;
	delay();
	Role[5]= role;
	send(Role, (sizeof(Role)));
	receive(6);
	i=0;

	delay();
	send(WriteChannel, (sizeof(WriteChannel)));
	receive(6);	
	i = 0;
	delay();
	send(WritePANID, sizeof(WritePANID));
	receive(6);

	delay();
	i = 0;
	send(WriteNetName, sizeof(WriteNetName));
	receive(6);
	i = 0;
	delay();
	send(WriteMLocPref, sizeof(WriteMLocPref));
	receive(6);
	i = 0;
	delay();
	send(WriteMK, sizeof(WriteMK));
	receive(6);
	i = 0;
	delay();
	send(WriteExtPID, sizeof(WriteExtPID));
	receive(6);
	i = 0;
	delay();
	send(WriteComCred, sizeof(WriteComCred));
	receive(6);
	i = 0;
	delay();
	send(WriteJoinCred, sizeof(WriteJoinCred));
	receive(6);
	delay();
	i=0;
	send(ifup,sizeof(ifup));
	receive(6);
	delay();
	i=0;
	send(WriteIP, (sizeof(WriteIP)));
	receive(6);
	delay();
	i=0;
	send(OpenSocket, (sizeof(OpenSocket)));
	receive(9);
	delay();
}
/******************* Main Function: *****************************/
void main()
{
  
   //---- Peripheral Configurations: -------------

   c = 0;
   flag = 0;
   _WS_Timer_Config(0x05);
   _WS_ADC_Config();
   _WSN_UART841_config();
   _WSN_ini_FPGA();
   //entrada cuenta humedad y temperatura por defecto

	InicioRed(leader);
	printf ("\nConnected\n\r");	   			   

 	while (1)
   	{
   	   if (flag == 1){

		send(SendHello, 31);	

		flag = 0;

		}				
   			 	   		  
   	}

}
/****************************************************************/
