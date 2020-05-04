¡¡ #include <SomeSerial.h>

SoftwareSerial mySoftwareSerial(8, 9); //RX, TX
SomeSerial Serial1(&mySoftwareSerial);

uint8_t sensordata[10];

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  while (!Serial) {}
  //while (!Serial1) {}
  Serial.println("c-base starts");
  initModem();
  Serial.println("DONE");
}

void sensorfake(){
  for (int i = 0; i < 10; i++){
    uint8_t d = sensordata[i];
    sensordata[i] = d+1;
  }
}

void sendit(String in) {
  Serial.print(in+"\r");
  Serial1.print(in+"\r");
  delay(100);
  while(Serial1.available() > 0){
    if(Serial1.available() > 0){
      byte c = Serial1.read();
      Serial.write(c);
    }
  }
}

void initModem(){
    sendit("AT");
    sendit("AT");
    sendit("AT+NRB");
    delay(5000);
    sendit("AT");
    sendit("AT");
    sendit("AT+NBAND=8");
    sendit("AT+CGDCONT=1,\"IP\",\"NBIOT.Telekom\"");
    sendit("AT+CEREG=2");
    sendit("AT");
    sendit("AT+CFUN=1");
    delay(5000);
    sendit("AT+COPS=1,2,\"12345\"");
    delay(5000);
    sendit("AT");
    delay(5000);
    sendit("AT");
    delay(5000);
    sendit("AT");
    sendit("AT+NPING=8.8.8.8");
    sendit("AT");
    sendit("AT+NSOCR=DGRAM,17,16666,1");
    delay(5000);
    sendit("AT");
}

void sendData(){

  String datastring = "AT+NSOST=0,ansi.23-5.eu,16666,11,01";

  for(uint8_t i = 0; i < 10; i++){
    String hexstring = String(sensordata[i], HEX);
  
    if(hexstring.length() < 2){
      hexstring = "0" + hexstring;
    }
    datastring+=hexstring;    
  }

  sendit(datastring);
}

void loop() {
  sensorfake();
  sendData();
  delay(2000);
}
