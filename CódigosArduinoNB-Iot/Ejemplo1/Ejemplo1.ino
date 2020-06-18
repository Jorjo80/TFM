//Para Arduino Uno

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); 

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("Wellcom to Dragino");
  mySerial.begin(9600);
  initModem();  
}

void loop() 
{   
  if (mySerial.available()>0){
      Serial.write(mySerial.read());
  }
  if (Serial.available()) {    
    mySerial.write(Serial.read());
  }
}
 

void sendit(String in)
{
  //Serial.println(in+"\r");
  Serial.println("\n");
  mySerial.print(in+"\r");
  delay(100);
  while(mySerial.available() > 0){
    if(mySerial.available() > 0){
      byte c = mySerial.read();
      Serial.write(c);
    }
  }
}

void initModem()
{
  sendit("ATE1");
  delay(100);
  sendit("ATI");
  delay(100);
}
