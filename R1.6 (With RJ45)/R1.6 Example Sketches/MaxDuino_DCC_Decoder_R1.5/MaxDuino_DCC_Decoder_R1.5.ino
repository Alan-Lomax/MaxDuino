int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 5;
char* about = "DCC Decoder Demo using NeoPixel and several Accessory Addresses";
/******************************************************************************
   Demo code for testing the MaxDuino board
   Uses the on board opto isolator to receive DCC serial data
   which the Arduino then decodes
      status messages are deisplayed on the serial monitor.
      NeoPixel illuminates based on switch addresses 5-10 being detected.
******************************************************************************
**  NOTE:                                                                   **
**  Some DCC controllers (eg DCC++EX) are connected by USB to a computer    **
**  If that same computer is then connected via USB to the MaxDuino         **
**  AND the MaxDuino is connected to DCC power a significant ground         **
**  loop can be established. The 7805 voltage regulator will get very hot   **
**  and the NANO could be damaged (USB interface circuitry).                **
**  ALWAYS USE an Isolated USB HUB when using this setup.                   **
******************************************************************************
  This code is adapted for MaxDuino from the DCC_decoder example given here: 
  https://github.com/aikopras/AP_DCC_library
  Hardware assumptions:
    MAXDuino PCB with DCC decoder circuitry in place. (Opto-Isolator sends data to pin D3)
    Arduino NANO
  Software Assumptions:
    Libraries Listed below are available
*/
#include <Arduino.h>
#include <AP_DCC_library.h>
#include <FastLED.h>
/*
  Note: 
    On the Arduino NANO
       Input D3 uses Int 1    << This is the default for the MaxDuino board
       Input D2 uses Int 0

    However:
      This test routine proved problematic through the versions of MaxDuino.
      This was not because this code was faulty ... but because when I tried to optimize
      pin usage for other features this one was bounced around - to a pin without the
      required interrupt capability. As of MaxDuino R1.3 this should be all sorted out.
        Rev 1.1 of the PCB had DCC decoding on pin D3 and it worked.
        Rev 1.2 of the PCB moved DCC decoding to pin D5 - which has no interrupt << Regression bug here DCC decoding will not work
        without reworking this sketch and its interrupt routine.  (PCINT perhaps)
        Rev 1.3 of the PCB moved DCC decoding back to pin D3. This sketch tests it out. 
        Rev 1.4 of the PCB added warning about ground loops after it was observed for first time. 
        Rev 1.5 of the PCB attempting to get meaningful colours on NeoPixel
   ******************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines and structures
//
const uint8_t dccPin = 3;  // DCC input is connected to Pin 3 (INT1)

extern Dcc dcc;           // Main object
extern Accessory accCmd;  // Data from received accessory commands

// How many leds in your strip?
#define NUM_LEDS 1  // And is only a single Neopixel
#define DATA_PIN 4  // Built in NeoPixel is on Pin 4
// Define the array of leds
CRGB leds[NUM_LEDS];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  printConfig(about);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  dcc.attach(dccPin);
  // The decoder can listen to one, or a range of addresses. In this example
  // we display details for all decoders with addresses between 1 and 100.
  // Note: One decoder address supports 4 switches / turnouts
  // Decoder 1   is switch   5..8
  // Decoder 100 is switch 397..400
  accCmd.setMyAddress(1, 100);
  delay(1000);
  Serial.println("Begin Test using AP_DCC_library");
  Serial.println("Use any accessory address from 1 to 100");
  Serial.println("Note: One decoder address supports 4 switches");
  Serial.println("Decoder 1 is switch 5..8");
  Serial.println("Decoder 100 is switch 397..400");
  delay(500);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if (dcc.input()) {
    switch (dcc.cmdType) {
      case Dcc::MyAccessoryCmd:
        // This section shows as a flicker on NeoPixel when command received.
        if (accCmd.command == Accessory::basic) {
          Serial.print(" Basic accessory command for my decoder address: ");
          leds[0] = CRGB::Cyan;
          FastLED.show();
        } else {
          Serial.print(" Extended accessory command for my decoder address: ");
          leds[0] = CRGB::Blue;
          FastLED.show();
        }
        //
        Serial.println(accCmd.decoderAddress);
        Serial.print(" - Turnout: ");
        Serial.println(accCmd.turnout);
        Serial.print(" - Switch number: ");
        Serial.print(accCmd.outputAddress);
        // Next section accounts for different turnout positions
        if (accCmd.position == 1) {
          leds[0] = CRGB::Green;
          FastLED.show();
          Serial.println(" +");
        } else {
          leds[0] = CRGB::Red;
          FastLED.show();
          Serial.println(" -");
        }
        if (accCmd.activate) {
          Serial.println(" - Activate");
        } else {
          Serial.println(" - Deactivate");
        }
        Serial.println();
        break;
      case Dcc::AnyAccessoryCmd:  // commands decoded but not in our range
        leds[0] = CRGB::Yellow;
        FastLED.show();
        break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  delay(500);  // pause half a second to allow text to exit the buffer
}
