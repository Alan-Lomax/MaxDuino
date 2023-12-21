int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 3;
char* about = "RS-485 Demo using NeoPixel and CMRI Address 0";
/******************************************************************************
   Demo code for testing the RS485 interface on the MAxDuino board
   Uses JMRI to initiate different colours on NeoPixel
   Hardware assumptions:
      MAXDuiono PCB R1.2 or later
      Arduino NANO
      MAX485 chip and related components are in place.
      USB - RS485 adaptor is available (For use on a PC running JMRI)
  Software Assumptions:
      Libraries Listed below are available
      JMRI is configured with Lights having addresses to match Arduino-CMRI address
 ******************************************************************************/
#include <Arduino.h>
#include <FastLED.h>
#include <Auto485.h>
#include <CMRI.h>
/******************************************************************************
The original version of this sketch plyed sounds using the DFRobot mini MP3 module.
To minimize testing complexity this was removed in R1.3.
The on board NeoPixel is used to indicate communications and it requires no extra setup.

Communications are done using RS485 connected to the on board MAX485 chip. There are two sets of
terminals on the PCB allowing both an in and an out connection. Terminals are electrically the same so it 
does not matter which pair you use (left or right). Do pay attention to the A and B signal lines on whichever pair you use.

There is a 120 ohm termination resistor on board (R7) which normally is open circuit due to EOL jumper being absent. 
If a shorting jumper is installed this resistor then becomes an in circuit End of Line resistor.
The jumper should only be installed on one MAXDuiono , preferably the farthest one from the USB connection.
Too many modules with R7 enabled will eventually cause communication issues.
For more info see: https://www.codrey.com/learn/rs-485-arduino-quick-primer/    especially about 'R7' 

***************
** JMRO/CMRI **
***************
Historically CMRI is custom hardware that was made to work with JMRI. 
These days with an Arduino more often than not it refers to an emulation of that older hardware using the CMRI library.
To download the library and review documentation see: https://github.com/madleech/ArduinoCMRI

****************
** JMRO SETUP **
****************
In JMRI First set up a serial connection in preferences. RS-485 is a relatively slow protocol. (19.2 Kbaud is good)
 
Then define your CMRI nodes (type = SMINI). The standard "SMINI" node has 24 inputs and 48 outputs
Each node is given an address that must match the one configured on the Arduino side. For this test it is address 0

In JMRI you can dnow use Tools/Tables to define either Sensors (inputs) or Lights(outputs)
These table entries will be given a hardware address that maps to a specific SMINI node, and bit.
    Input address    1-  24 or output address    1-  48 are on CMRI_ADDR 0
    Input address 1000-1024 or output address 1000-1048 are on CMRI_ADDR 1
    Input address 2000-2024 or output address 2000-2048 are on CMRI_ADDR 2
    Input address 3000-3024 or output address 3000-3048 are on CMRI_ADDR 3
  etc

In your loop code these lines are key for correct operation checking or setting each bit.
    cmri.process();          // Is the main processing call to make CMRI work
    cmri.set_bit(x, true);   // Will set bit 'x' to true or false. Setting a bit here means it is an input in JMRI (a sensor)
    cmri.get_bit(y);         // Will retrieve the current value of bit 'y' as true/false. Getting a bit means it is a light in JMRI

set_bit(x, true) and get_bit(y) are working with different arrays so even if x = y they are not referring to the same bit     

Notes:
 One Arduino is not restricted to emulating just one CMRI node. 
 Declare a second node address and instantiate a second CMRI/SMini node
 
 Physically the DE and RE pins control the direction of traffic on a MAX-485 chip.
 There are actually two pins on the chip and each is controlled by a separare Arduino pin. 
 This allows both to set to high impedance mode - and so in circuit programming is possible without
 disrupting the entire RS485 network. 

 ******************************************************************************/
#define CMRI_ADDR 0                 // Which SMINI node address are using for this MaxDuino
#define DE_PIN 2                    // Maxduino pin D2 is the RS-485 Driver   Output Enable
#define RE_PIN 5                    // Maxduino pin D3 is the RS-485 Receiver Output Enable
Auto485 bus(DE_PIN, RE_PIN);        // A of pcb Rev 1.2 now use the different pins for DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);  // As shown allows explicit numbers. SMINI = 24 inputs, 48 outputs//
// How many leds in your strip?
#define DATA_PIN 4  // Built in NeoPixel is on Pin 4
#define NUM_LEDS 1  // And is only a single Neopixel

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(19200);                                  // for DEBUG console
  printConfig(about);                                   // display basic Info about program
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  // Starting the RS485 bus (line below). Note this prevents normal serial debug console messages.
  bus.begin(19200, SERIAL_8N2);  // Start RS485 Comms at 19.2 Kbps, 8 data bits, no parity, 2 stop bits.
}

void loop() {
  cmri.process();  // The main processing node of cmri library

  int Activated = 99;            // To hold which bit is activated (99 means none)
  for (int x = 0; x < 7; x++) {  // loop through some possible CMRI bits
    if (cmri.get_bit(x)) {       // If the bit in the light table is found to be set
      cmri.set_bit(x, true);     // Echo back the bit in the sensor table (loop back test)
      Activated = x+1;           // capture the last bit that was activated
    } else {
      cmri.set_bit(x, false);  // Clear the bit in the sensor table (loop back test)
    }
  }

  // Below covers 7 CMRI inputs and displays distinct colours.
  switch (Activated) {
    case 1:
      leds[0] = CRGB::Red;
      break;
    case 2:
      leds[0] = CRGB::DarkGreen;
      break;
    case 3:
      leds[0] = CRGB::Cyan;
      break;
    case 4:
      leds[0] = CRGB::Magenta;
      break;
    case 5:
      leds[0] = CRGB::Blue;
      break;
    case 6:
      leds[0] = CRGB::Lime;
      break;
    case 7:
      leds[0] = CRGB::Yellow;
      break;
    default:
      leds[0] = CRGB::Black;
      break;
  }
  FastLED.show();
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
  delay(500);  // pause half a second to allow text to exit the buffer
}
