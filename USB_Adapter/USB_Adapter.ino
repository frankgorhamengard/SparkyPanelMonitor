/*This is code for the Auxiliary Board
This sketch reads communicates with a Sparky Control Panel at 600 baud on the Alternate port

It has a debug flag turned off for normal running.

It has controlled 5 bar LED showing activity in the panel.

Developed by Miss Daisy FRC Team 341
 
*/
#define DEBUG false

#include <AltSoftSerial.h>
AltSoftSerial altser;
// D8 - AltSoftSerial RX
// D9 - AltSoftSerial TX

#define BLINKLED_13        13 
#define VINPIN_A0 A0
unsigned long triggerTime;
const int panelLedArr[5] = {5,6,9,10,11}; //map of wired pins to LEDs
int incomingChar;
int uutChar;
int charCount;

/////////////////////////////////////////////////////////////////////////////////
// called once at start
void setup(){
  Serial.begin(9600);
  altser.begin(600);   // to AltSoftSerial RX
  
  while (!Serial) delay(50);  // wait for Arduino Serial Monitor
  Serial.println("Sparky USB Test Monitor");
  Serial.println(__DATE__);

  pinMode(13   , OUTPUT);
  digitalWrite(13   , HIGH);
  
  triggerTime = millis() + 3000;
  delay(2000);
}

void printHelp (void) {
  Serial.println("========================================");
  Serial.println("Select mode: ");
  Serial.println(" 1  600 baud serial");
  Serial.println(" 2  servo detect");
  Serial.print("?:");
// Serial.println("  ");
}

//    declare functions
void runSerialMonitor(void);
void runServoDetect(void);
void localCircuitTest(void);

/////////////////////  MAIN LOOP  /////////////////////////////
void loop() {
  printHelp();
  for (;;) { 
    incomingChar = Serial.read();
    if ( incomingChar >= 0 ) {
      Serial.write(incomingChar);
      switch (incomingChar)
      {
        case '1': 
          runSerialMonitor();
          printHelp();
          break;
        case '2': 
          runServoDetect();
          printHelp();
          break;
         case '#': 
          Serial.println(charCount);
          printHelp();
          break;
         case '9': 
          localCircuitTest();
          printHelp();
          break;
         case 'H': 
         case 'h': 
          printHelp();
          break;
        default:
          altser.write(incomingChar);
      }
    }
    //  delay for good measure
    delay(10);
  }
}

///////////////  runSerialMonitor ////////////////////////////////////////////////
void runSerialMonitor(void) {
  int inchar;
  Serial.println("---- Serial monitor, q to quit------");
  while ( (inchar=Serial.read()) != 'q' ) { // until quit is hit
    if ( inchar > 0 ) {
      altser.write(inchar);
    }
    uutChar = altser.read();
    if ( uutChar >= 0 ) {
      charCount++; 
      Serial.write(uutChar);
    }
  }
}

/////////////////   runServoDetect   ////////////////////////////////////////////
void runServoDetect(void) {
 unsigned long testnow;     // milliseconds
// unsigned long pulseStart;    // microseconds
// unsigned long pulseEnd;      // microseconds
 unsigned long pulseLength;             // microseconds  
 
  Serial.println("servo detect on pin 2");
  triggerTime = millis()+1000;
  Serial.println("---- Servo monitor, q to quit------");
  while ( Serial.read() != 'q' ) { // until quit is hit
    testnow = millis();
    // once per period 
    if ( testnow >= triggerTime ) {  // time to start next sample
        triggerTime = testnow + 1000; // period
        pulseLength = pulseIn(2, HIGH);
        Serial.println();    // start on newline
        Serial.print( pulseLength );
        Serial.print( "     "); 
        Serial.print( (pulseLength-1000)/10 );
        Serial.print( " %   "); 
        Serial.print( ((pulseLength-1000)/11)*2 );
        Serial.print( " #   "); 
      }
    delay(10);
  }
}

unsigned long testnow;

/////////////////  localCircuitTest   //////////////////////////////////////////////
void localCircuitTest(void) {
  Serial.println("---- Local monitor, q to quit------");
  while ( Serial.read() != 'q' ) { // until quit is hit
    testnow = millis();
    // once per period 
    if ( testnow >= triggerTime ) {
      triggerTime = testnow + 100; //period
      int value1 = (analogRead(A1)+3)>>2;   
      Serial.print( value1 );
   }
  }
}
