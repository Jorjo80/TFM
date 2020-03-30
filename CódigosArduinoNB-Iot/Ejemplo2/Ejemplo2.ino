//Para arduino Leonardo
void setup() {
  // initialize both serial ports:
  Serial1.begin(9600);
  Serial.begin(9600);
  while (!Serial1) {
     ;
  }
  Serial.println("Welcome to use Dragino NB-IoT Shield");
}
 
void loop() {
  // read from port 1, send to port 0:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
  // read from port 0, send to port 1:
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
 }
}
