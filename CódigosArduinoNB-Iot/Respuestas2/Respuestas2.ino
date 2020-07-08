//Para Arduino Uno

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); 

bool Prim = false;
bool Seg = false;
bool OK = false;
String Frase=" ";

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
  while(mySerial.available() > 0)
  {
    if(mySerial.available() > 0)
    {
      char c = mySerial.read();
      Serial.write(c);
      OK=RespuestaOK(c);
    }
  }
  lecturaFrase();
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
    return false;
  }
}

//Para detectar OK
bool RespuestaOK(char c)
{
  if(Prim == true && c =='K')
  {
    Frase.concat(c);
    Seg = true;
  }
  else
  {
    Prim = false;
    //return false;
  }       
  if(Respuesta(c, 'O'))
  {
    Frase = c;
    Prim = true;
  }
  if(Respuesta(c, '\r')&& Seg == true)
  {
    Prim = false;
    Seg = false;
    return true;
  }           
}

void lecturaFrase()
{
  char a;
  int i = 0;
  Frase.concat('\0');
  //Serial.print("Frase Resultado = ");
  //Serial.print(Frase+'\n');
  while(a != '\0')
  {
    a = Frase[i];
    i++;
  }

  if(Frase == "OK")
  {
    Serial.println("todo ok\n");
    Frase = ' ';
  }
}
void sendit(String in)
{
  
  
  //Serial.println(in+"\r");
  OK = false;
  mySerial.print(in+"\r");
  delay(100);
  while(mySerial.available() > 0)
  {
    if(mySerial.available() > 0)
    {
      char c = mySerial.read();
      Serial.write(c);
      OK=RespuestaOK(c);
    }
  }
  lecturaFrase();
}


void initModem()
{
  sendit("ATE1");
  delay(100);
  if(OK == false)
    exit;
  sendit("ATI");
  delay(100);
}
