int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 3;
// Testing all inputs except for A6/A7, D0-D4, D13
char* about = "I/O Header Pin Testing";
/******************************************************************************
   Demo code for testing the MAxDuino board (I/O pin headers)
   Defines all pins as inputs and then uses those pins to light  LED_BUILTIN and
   also the on board Neopixel. 
   The colour of the Neopixel is different for each input.
   The Purpose of this is simply to allow a quick test of the I/O header pins
   to ensure the board is working properly.
   The colour is not important as long as a change is observed.
   If no input is detected the NeoPixel should be black. (no light emitted)

   Hardware assumptions:
      MAXDuiono PCB with single bare chip WS2812 NeoPixel in place.
      I/O header pins installed
      Arduino NANO
  Software Assumptions:
      Libraries Listed below are available

  Test Plan
      Use a 3 wire sensor to activate each MaxDuino I/O pin in turn. 
      Alternate: Use a single jumper cable with two female dupont connectors
      and connect each input pin to ground one at a time.    
 ******************************************************************************/
#include <FastLED.h>

/*
  This routine defines **most** of the header pins as inputs.
  It reads them and outputs a different colour to the NeoPixel according to which one was detected.
  On the MaxDuiono choices are   4,5,6,7  8,9,13   A0,A1,A2,A3   A4,A5,A6,A7
                                 =======  ======   ===========   ===========
  The grouping above shows header arrangement for the pins.
  
  Notes: 
  **Pins Not Tested**
  Pins D0 and D1 (Rx/Tx) communications are not brought out to header pins.
  Pin  D2 is used for RS485 Comms  and is not brought out to a header pin. (Test with RS485)
  Pin  D4 is available on the headers. Optionally an external NeoPixel strip could be connected (Max of 10 elements).
  Pin  D5 is used for RS485 Comms  and is not brought out to a header pin. (Test with RS485)
  Pin D13 is also available on the headers but this is the LED_BUILTIN so additional testing is of limited value.
  Because pins A6 and A7 are analog inputs that lack digital input capability they are not tested.
  
  **Pins Tested**
  Per list defined below.
*/
// For NeoPixel
#define NUM_LEDS 11  // How many NeoPixels in the strip? (one on board plus up to 10 external)
#define DATA_PIN 4   // An external NeoPixel strip could be connected to this header also.
//  Define Input Pins
#define Input_pin1 3  // Available on the headers but note this pin is also used for DCC Comms. (Will also test with DCC)
#define Input_pin2 6
#define Input_pin3 7
#define Input_pin4 8
#define Input_pin5 9
// #define Input_pin6 13 // Pin 13 not used here. (LED_BUILTIN)
#define Input_pin7 A0
#define Input_pin8 A1
#define Input_pin9 A2
#define Input_pin10 A3
#define Input_pin11 A4
#define Input_pin12 A5
// Pin A6 not used here. (no digital input)
// Pin A7 not used here. (no digital input)

#define activated false  // some sensors are active low, some are active high. This allows a quick toggle

int currentInput = 0;   // variable to hold the currently read input
int previousInput = 0;  // variable to hold the prior input so we can report on change only

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(19200);
  printConfig(about);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  // Set all pin modes
  pinMode(Input_pin1, INPUT_PULLUP);
  pinMode(Input_pin2, INPUT_PULLUP);
  pinMode(Input_pin3, INPUT_PULLUP);
  pinMode(Input_pin4, INPUT_PULLUP);
  pinMode(Input_pin5, INPUT_PULLUP);
  // pinMode(Input_pin6, INPUT_PULLUP);
  pinMode(Input_pin7, INPUT_PULLUP);
  pinMode(Input_pin8, INPUT_PULLUP);
  pinMode(Input_pin9, INPUT_PULLUP);
  pinMode(Input_pin10, INPUT_PULLUP);
  pinMode(Input_pin11, INPUT_PULLUP);
  pinMode(Input_pin12, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  currentInput = 0;
  if (digitalRead(Input_pin1) == activated) { currentInput = 1; }
  if (digitalRead(Input_pin2) == activated) { currentInput = 2; }
  if (digitalRead(Input_pin3) == activated) { currentInput = 3; }
  if (digitalRead(Input_pin4) == activated) { currentInput = 4; }
  if (digitalRead(Input_pin5) == activated) { currentInput = 5; }
  //  if (digitalRead(Input_pin6) == activated) { currentInput = 6; }
  if (digitalRead(Input_pin7) == activated) { currentInput = 7; }
  if (digitalRead(Input_pin8) == activated) { currentInput = 8; }
  if (digitalRead(Input_pin9) == activated) { currentInput = 9; }
  if (digitalRead(Input_pin10) == activated) { currentInput = 10; }
  if (digitalRead(Input_pin11) == activated) { currentInput = 11; }
  if (digitalRead(Input_pin12) == activated) { currentInput = 12; }

  if (previousInput != currentInput) {  // Report only on change of value
    Serial.print("Current Input: ");
    Serial.println(currentInput);
    previousInput = currentInput;
  }

  // below covers all of the inputs with distinct colours.
  // As long as a colour change is obseved then the pin header is obviously working.
  switch (currentInput) {
    case 0:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      break;
    case 1:
      leds[0] = CRGB::DeepPink;
      leds[1] = CRGB::DeepPink;
      break;
    case 2:
      leds[0] = CRGB::Blue;
      leds[2] = CRGB::Blue;
      break;
    case 3:
      leds[0] = CRGB::Yellow;
      leds[3] = CRGB::Yellow;
      break;
    case 4:
      leds[0] = CRGB::SeaGreen;
      leds[4] = CRGB::SeaGreen;
      break;
    case 5:
      leds[0] = CRGB::Lime;
      leds[5] = CRGB::Lime;
      break;
    case 6:
      leds[0] = CRGB::Red;
      leds[6] = CRGB::Red;
      break;
    case 7:
      leds[0] = CRGB::Cyan;
      leds[7] = CRGB::Cyan;
      break;
    case 8:
      leds[0] = CRGB::Green;
      leds[8] = CRGB::Green;
      break;
    case 9:
      leds[0] = CRGB::Magenta;
      leds[9] = CRGB::Magenta;
      break;
    case 10:
      leds[0] = CRGB::DarkCyan;
      leds[10] = CRGB::DarkCyan;
      break;
    case 11:
      leds[0] = CRGB::HotPink;
      leds[1] = CRGB::HotPink;
      leds[10] = CRGB::HotPink;
      break;
    case 12:
      leds[0] = CRGB::Purple;
      leds[1] = CRGB::Purple;
      leds[2] = CRGB::Purple;
      break;
    default:
      leds[0] = CRGB::Red;
      leds[1] = CRGB::Red;
      leds[3] = CRGB::Red;
      leds[5] = CRGB::Red;
      leds[7] = CRGB::Red;
      leds[9] = CRGB::Red;
      break;
  }
  // This is for the NeoPixel (on board plus any externals on Header pin 4)
  FastLED.show();
  // Below is in addition to NeoPixel
  digitalWrite(LED_BUILTIN, (currentInput != 0));  // any non zero value turns on the builtin LED
}

void printConfig(const char* about) {
  /* 
    ************************************
    ** printConfig(const char* about) **
    ************************************

   A subroutine that prints out basic information about the running program.
   Intended to be called once early in the startup code.  When connected to
   serial monitor this reveals what program is actually loaded on a given Arduino.

   Prior to invoking (typically on the first four lines of your sketch)
   the following four variables must be defined. Alter the numbers to suit of course.

  int VER_MAJ = 0;
  int VER_MIN = 0;
  int VER_BETA = 1;
  char* about = "a few words about the sketch";
*/
  Serial.println(">");
  // Source code
  Serial.println(F(">         File = " __FILE__));
  Serial.print(F(">        About = "));
  Serial.println(about);

  // code version
  Serial.print(F("> Code version = "));
  Serial.print(VER_MAJ);
  Serial.print(F("."));
  Serial.print(VER_MIN);
  Serial.print(F("."));
  Serial.println(VER_BETA);

  // Compiler version
  Serial.println(F("> Compiled on  = " __DATE__ " at " __TIME__));
  Serial.print(F("> Compiler ver = "));
  Serial.println(__cplusplus);

  // copyright
  Serial.println(">");
  Serial.println(F(">    For MaxDuino by by Alan Lomax (MERG M8640) Oct 2023"));
  Serial.println(">");
}
