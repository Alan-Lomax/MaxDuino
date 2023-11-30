int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 3;
char* about = "OnBoard NeoPixel Testing";
/******************************************************************************
   Demo code for testing the MAxDuino board using the on board Neopixel
   
   Hardware assumptions:
      MAXDuiono PCB with single bare chip WS2812 NeoPixel in place.
      Arduino NANO
  Software Assumptions:
      Libraries Listed below are available
 ******************************************************************************/
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 1

// For led chips like WS2812, which have a data line, ground, and power
// you just need to define DATA_PIN.
#define DATA_PIN 4

// Define the array of leds (in this case onmly one element is needed)
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(19200);
  printConfig(about);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);// GRB ordering is assumed
}

void loop() {
  // Turn the LED on, then pause
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  // Turn the LED on, then pause
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  // Turn the LED on, then pause
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}

void printConfig(const char* about) {
  /* 
    ***************************************
    ** printConfig(const char* about)    **
    ***************************************

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
