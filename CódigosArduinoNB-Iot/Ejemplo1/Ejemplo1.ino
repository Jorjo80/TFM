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

bool Respuesta(char letra, char msj)
{
  if(letra == msj)
  {
    return true;
  }
  else
  {
    //Serial.write("Mal");
    return false;
  }
}

void sendit(String in)
{
  //Serial.println(in+"\r");
  bool Prim = false;
  bool Seg = false;
  bool todo;
  Serial.println("\n");
  mySerial.print(in+"\r");
  delay(100);
  while(mySerial.available() > 0)
  {
    if(mySerial.available() > 0)
    {
      char c = mySerial.read();
      Serial.write(c);
      if(Prim == true)
      {
        Seg=Respuesta(c, 'K');
      }
      else
      {
        Prim = false;
      }
      
      Prim=Respuesta(c,'O');
      if(Seg == true)
      {
        todo = true;
      }
      else
      {
        todo = false;
      }
      if(todo == true)
      {
        Serial.write("\nRecibido Todo Bien");
        todo = false;
        Prim = false;
        Seg = false;        
      }
     if(c== '\n')
      Serial.write("fin");
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
