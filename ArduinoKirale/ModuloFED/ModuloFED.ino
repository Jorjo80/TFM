#include <SoftwareSerial.h>
#include <PacketSerial.h>
#include <Packetizer.h>

byte ifup[] = {0x00, 0x00, 0x10, 0x08, 0x18};
byte ifdown[] = {0x00, 0x00, 0x10, 0x07, 0x17};
byte ComClear[] = {0x00, 0x00, 0x10, 0x00, 0x10};
byte Reset[] = {0x00, 0x00, 0x10, 0x03, 0x13};
byte Status[] = {0x00, 0x00, 0x11, 0x05, 0x14}; //Look the answer in the KBI pdf
byte ReadChannel[] = {0x00, 0x00, 0x11, 0x12, 0x03};
byte WriteChannel[] = {0x00, 0x01, 0x10, 0x12, 0x0c, 0x0F}; //Channel 15 predefined, change last byte to change channell between 11-26
byte ReadRole[] = {0x00, 0x00, 0x11, 0x19, 0x08};
byte WriteRole[] = {0x00, 0x01, 0x10, 0x19, 0x0b, 0x03}; //FED, look the table below and change the last byte

/*
0 = Not Configured
1 = Router 
2 = REED (Router Elegible End Device
3 = FED (Full End Device)
4 = MED (Minimal End Device)
5 = SED (Sleepy End Device) 
6 = Leader

Modifications conditions
Status must be none, except none - saved configuration. Also allowed when status is joined to trigger some
role transition mechanisms (SED → MED, SED → FED, MED→ FED).
*/

byte ReadJoinCred[] = {0x00, 0x00, 0x11, 0x17, 0x06};
byte WriteJoinCred[] = {0x00, 0x00, 0x10, 0x17, 0x06, 0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30,0x30, 0x30, 0x30, 0x35, 0x46, 0x41}; //In order to write the Joiner Credential, add the packets after 0x6d



typedef enum
{
  Unused = 0x00, /* Unused (framing character placeholder) */
  DiffZero = 0x01, /* Range 0x01 - 0xCE: */
  DiffZeroMax = 0xCF, /* n-1 explicit characters plus a zero */
  Diff = 0xD0, /* 207 explicit characters, no added zero */
  Resume = 0xD1, /* Unused (resume preempted packet) */
  Reserved = 0xD2, /* Unused (reserved for future use) */
  RunZero = 0xD3, /* Range 0xD3 - 0xDF: */
  RunZeroMax = 0xDF, /* 3-15 zeroes */
  Diff2Zero = 0xE0, /* Range 0xE0 - 0xFE: */
  Diff2ZeroMax = 0xFE, /* 0-30 explicit characters plus 2 zeroes */
  Error = 0xFF /* Unused (PPP LCP renegotiation) */
}StuffingCode;


PacketSerial_<COBS, 0, 512> myPacketSerial;
SoftwareSerial mySoftwareSerial(10, 11); //Rx - TX


void setup()
{
    Serial.begin(115200);
    mySoftwareSerial.begin(115200);
    myPacketSerial.setStream(&mySoftwareSerial);
    myPacketSerial.setPacketHandler(&onPacketReceived);
    InicioFED();
}

void loop()
{
  Packetizer::parse();
  myPacketSerial.update();
  if (myPacketSerial.overflow())
    {

        Serial.println("You must increase buffer size");
        // Send an alert via a pin (e.g. make an overflow LED) or return a
        // user-defined packet to the sender.
        //
        // Ultimately you may need to just increase your recieve buffer via the
        // template parameters.
    }
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  uint8_t tempBuffer[size];
  memcpy(tempBuffer,buffer,size);
  Serial.print("Respuesta = ");
  Serial.print(" tamaño buffer es = ");
  Serial.println(sizeof(tempBuffer));
  for(int i = 0; i < sizeof(tempBuffer)/sizeof(tempBuffer[0]); i++)
  {
    Serial.print("0x");
    Serial.print(tempBuffer[i],HEX);
    Serial.print(" : ");
  }
  Serial.println();
}

void InicioFED()
{
  /*myPacketSerial.send(ComClear,sizeof(ComClear)/sizeof(ComClear[0]));
  Serial.println("RealizadoClear");
  myPacketSerial.update();  
  delay(3000);*/
  myPacketSerial.send(WriteChannel,sizeof(WriteChannel)/sizeof(WriteChannel[0]));
  Serial.println("Escrito Canal");
  while ( mySoftwareSerial.available()==0) {}
  myPacketSerial.update(); 
  delay(1000);
  myPacketSerial.send(WriteRole,sizeof(WriteRole)/sizeof(WriteRole[0]));
  Serial.println("WriteRole");
  while ( mySoftwareSerial.available()==0){}
  myPacketSerial.update();
  delay(1000);  
  myPacketSerial.send(ifup,sizeof(ifup)/sizeof(ifup[0]));
  Serial.println("IFUP");
  while ( mySoftwareSerial.available()==0){}
  myPacketSerial.update(); 
  
}
