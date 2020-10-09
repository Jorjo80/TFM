/* reserve 200 bytes for the inputString:
 * assuming a maximum of 200 bytes
 */
uint8_t inputString[200];  // a String to hold incoming data
int countInput = 0;
bool stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(115200);
}

void loop() {
  //serialEvent();
  // print the string when 0x16 arrives:
  if (stringComplete) {
    for (int i=0; i<countInput; i++) {
      Serial.print(inputString[i], HEX);
      Serial.print(" ");
    }
    // clear the string:
    countInput = 0;
    stringComplete = false;
  }
}
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    inputString[countInput] = (uint8_t)Serial.read();

    // if the incoming character is '0x16', set a flag so the main loop can
    // do something about it:
    if (inputString[countInput] == 0x16) {
      stringComplete = true;
    }
    countInput++;
  }
}
