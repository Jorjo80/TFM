#include <SoftwareSerial.h>
#include <PacketSerial.h>


byte ifup[] = {0x00, 0x00, 0x10, 0x08, 0x18};
byte ifdown[] = {0x00, 0x00, 0x10, 0x07, 0x17};
byte ComClear[] = {0x00, 0x00, 0x10, 0x00, 0x10};
byte Reset[] = {0x00, 0x00, 0x10, 0x03, 0x13};
byte Status[] = {0x00, 0x00, 0x11, 0x05, 0x14}; //Look the answer in the KBI pdf
byte ReadChannel[] = {0x00, 0x00, 0x11, 0x12, 0x03};
byte WriteChannel[] = {0x00, 0x01, 0x10, 0x12, 0x0c, 0x0F}; //Channel 15 predefined, change last byte to change channell between 11-26
byte ReadRole[] = {0x00, 0x00, 0x11, 0x19, 0x08};
byte WriteRole[] = {0x00, 1, 16, 25, 11, 3}; //FED, look the table below and change the last byte

/*
  0 = Not Configured
  1 = Router -- CKS = 09
  2 = REED (Router Elegible End Device) -- CKS = 0A
  3 = FED (Full End Device) -- CKS = OB
  4 = MED (Minimal End Device) -- CKS = 0C
  5 = SED (Sleepy End Device) CKS = 0D
  6 = Leader -- CKS = 0E

  Modifications conditions
  Status must be none, except none - saved configuration. Also allowed when status is joined to trigger some
  role transition mechanisms (SED → MED, SED → FED, MED→ FED).
*/

byte ReadJoinCred[] = {0x00, 0x00, 0x11, 0x17, 0x06};
byte WriteJoinCred[] = {0x00, 0x10, 0x10, 0x17, 0x69, 0x38, 0x34, 0x30, 0x34, 0x44, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x46, 0x43};



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
} StuffingCode;


PacketSerial_<COBS, 0, 512> InterfazModulo;
SoftwareSerial Modulo(10, 11); //Rx - TX


void setup()
{
  Serial.begin(115200);
  Modulo.begin(115200);
  InterfazModulo.begin(115200);
  InterfazModulo.setStream(&Modulo);
  InterfazModulo.setPacketHandler(&onPacketReceived);
  InicioFED();
}

void loop()
{

  
  //InterfazModulo.update();
  if (InterfazModulo.overflow())
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
  int variable = 5;
  //size = sizeof(buffer);
  uint8_t tempBuffer[size];

  memcpy(tempBuffer, buffer, size);
  Serial.print("Respuesta = ");
  Serial.print(" tamaño buffer es = ");
  Serial.println(size);
  for (int i = 0; i < size; i++)
  {
    Serial.print("0x");
    Serial.print(buffer[i], HEX);
    Serial.print(" : ");
  }
  Serial.println();
}

void InicioFED()
{
  /*InterfazModulo.send(ComClear,sizeof(ComClear)/sizeof(ComClear[0]));
    Serial.println("Realizado Clear");
    while ( Modulo.available() == 0) {}
    InterfazModulo.update();
    delay(1000);*/
  //Modulo.write(WriteChannel, sizeof(WriteChannel) / sizeof(WriteChannel[0]));
  int i = 0;
  InterfazModulo.send(WriteChannel, sizeof(WriteChannel) / sizeof(WriteChannel[0]));
  Serial.println("Escrito Canal");
  while ( Modulo.available() == 0) {}
  InterfazModulo.update();
  
    //while ( Modulo.available() > 0) {};
    InterfazModulo.send(WriteRole, sizeof(WriteRole) / sizeof(WriteRole[0]));
    Serial.println("Write Role");
    while ( Modulo.available() == 0) {}
    InterfazModulo.update();
    if (InterfazModulo.overflow())
    {

    Serial.println("You must increase buffer size");
    // Send an alert via a pin (e.g. make an overflow LED) or return a
    // user-defined packet to the sender.
    //
    // Ultimately you may need to just increase your recieve buffer via the
    // template parameters.
    }
    InterfazModulo.send(WriteJoinCred, sizeof(WriteJoinCred) / sizeof(WriteJoinCred[0]));
    Serial.println("Write Join Cred");
    while ( Modulo.available() == 0) {}
    InterfazModulo.update();
    if (InterfazModulo.overflow())
    {

    Serial.println("You must increase buffer size");
    // Send an alert via a pin (e.g. make an overflow LED) or return a
    // user-defined packet to the sender.
    //
    // Ultimately you may need to just increase your recieve buffer via the
    // template parameters.
    }
    //while ( Modulo.available() > 0) {};
    InterfazModulo.send(ifup, sizeof(ifup) / sizeof(ifup[0]));
    Serial.println("IFUP");
    while ( Modulo.available() == 0) {}
    InterfazModulo.update();
    if (InterfazModulo.overflow())
    {

    Serial.println("You must increase buffer size");
    // Send an alert via a pin (e.g. make an overflow LED) or return a
    // user-defined packet to the sender.
    //
    // Ultimately you may need to just increase your recieve buffer via the
    // template parameters.
    }
}

byte XORChecksum8(const byte *data, size_t dataLength)
{
  byte value = 0;
  for (size_t i = 0; i < dataLength; i++)
  {
    value ^= (uint8_t)data[i];
  }
  return ~value;
}
