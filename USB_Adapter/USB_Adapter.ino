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
#define SYSTEM_ENABLE_12   12
#define PANEL_LED5_11     11
#define PANEL_LED4_10     10
#define PANEL_LED3_9     9     //do not use, soft serial tx
#define SHOOT_BUTTON_8    8       //do not use, soft serial rx
#define INTAKE_BUTTON_7   7
#define PANEL_LED2_6     6
#define PANEL_LED1_5     5
#define TEST_SWITCH_4     4
#define R_STICK_BUTTON_3  3
#define L_STICK_BUTTON_2  2

#define STICK2X      0
#define STICK2Y      1
#define STICK1X      2
#define STICK1Y      3
#define SHOOTERSPEED 4

unsigned long triggerTime;
const int panelLedArr[5] = {5,6,9,10,11}; //map of wired pins to LEDs
int incomingChar;
int uutChar;
int charCount;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION: setLED,   returns nothing
// ARGUMENTS: LEDnum is value 0 to 4, brightness is 0 (off) to 255 (full on)
void setLED(int LEDnum, unsigned int brightness) {
  if ( brightness > 255 ) brightness = 255;
  unsigned long brightness_l = brightness & 255; 
  //  index to pins, use panelLedArr   ,  value to write is non-linear
  if ( 0 <= LEDnum < 5 ) {
    int LEDoutput = min( 255-((brightness_l+7)/8), (255-brightness_l)*3 );
    analogWrite( panelLedArr[LEDnum], LEDoutput );
  }
}
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
  
  // init inputs and enable pullup
  pinMode(SYSTEM_ENABLE_12   , INPUT_PULLUP); // LOW is all stop
//  pinMode(SHOOT_BUTTON    , INPUT_PULLUP); // LOW is SHOOT
  pinMode(INTAKE_BUTTON_7   , INPUT_PULLUP); // LOW is INATKE
  pinMode(TEST_SWITCH_4     , INPUT_PULLUP); // LOW is on
  pinMode(R_STICK_BUTTON_3  , INPUT_PULLUP); // LOW is active
  pinMode(L_STICK_BUTTON_2  , INPUT_PULLUP); // LOW is active

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
  while ( inchar=Serial.read() != 'q' ) { // until quit is hit
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
 unsigned long pulseStart;    // microseconds
 unsigned long pulseEnd;      // microseconds
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
      int value1 = (analogRead(SHOOTERSPEED)+3)>>2;    ///STICK1Y)+3)>>2;
      // int value2 = (analogRead(STICK2Y)+3)>>2;
      if ( value1 < 128 ) {
        setLED( 0, (127-value1)*2 );
        setLED( 1, 0);
      } else {
        setLED( 0, 0);
        setLED( 1, (value1-128)*2 );
      }
      // check is display test on
      if ( digitalRead(TEST_SWITCH_4) == LOW ) { // LOW is on
        //set our LED on or off based on 4   
        digitalWrite(13, HIGH);
      } else {
        digitalWrite(13, LOW);  
      }
      setLED( 4, digitalRead(12)*250 );
    }
  }
}
