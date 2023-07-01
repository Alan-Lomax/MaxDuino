int VER_MAJ = 0;
int VER_MIN = 0;
int VER_BETA = 3;
/******************************************************************************
   Demo code for testing the MAxDuino board
   Uses the on board MAX485 8 pin chip to initiate tracks on MP3 player module.
   Hardware assumptions:
      MAXDuiono PCB
      Arduino NANO
      DFRobot MP3 player and related components are in place
      SD card with a folder called MP3 containing 2 to 7 MP3 files
      MAX485 chip and related components are in place.
      USB - RS485 adaptor is available (For use on a PC running JMRI)
  Software Assumptions:
      Libraries Listed below are available
      JMRI is configured with Lights having addresses to match Arduino-CMRI address
 ******************************************************************************/
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Auto485.h>
#include <CMRI.h>
/******************************************************************************
Uses the DFRobot mini MP3 module.  Clones of this module (like the MP-TF-16P) did not work for me
Clone did not respond to some serial commands like setting the volume. (possibly it was my error but I moved on)
The SD card must have several tracks on it in an MP3 folder. Sample tracks included with sketch.

Communications are done using RS485 connected to the on board MAX485 chip. There are two sets of
terminals on the PCB allowing both an in and an out connection. Terminals are electrically the same so it 
does not matter which pair you use (left or right). Do pay attention to the A and B signal lines on whichever pair you use.
There is a 120 ohm termination resistor on board which normally is open circuit (Thus having no effect) 
If a shorting jumper is installed on the pins marked EOL (End of Line) this resistor beomes an End of Line resistor.
The jumper should only be installed on one MAXDuiono , preferwbly the farthest one from the USB connection.
For the distances involved in model railways the actual location is unlikely to be an issue
However please ensure only one board has the EOL enabled else RS-485 may prove unreliable under higher speeds and loads. 

JMRO/CMRI
CMRI was at one time custom hardware that worked with JMRI. These days with an Arduino 
more often than not it refers to an emulation of that older hardware using the CMRI library.
To download the library and review documentation see: https://github.com/madleech/ArduinoCMRI
 
Notes:
 One Arduino is not restricted to emulating just one CMRI node. Declare a second node address and instantiate a second CMRI/SMini node
 
 Physically the DE pin controls the direction of traffic on a MAX-485 chip.
 There are actually two pins on the chip, but we use the same Arduino pin for both at the same time. 
 The RS-485 protocol cannot send and receive at same time. (it is half duplex)

 In JMRI you first set up a serial connection in preferences. RS-485 is a relatively slow protocol. (19.2 Kbaud is good)
 The RS-485 spec calls for two 120 ohm resistors as 'terminators. A lot of external modules have this resistor
 built in to every module. Putting too many modules on the bus will eventually cause communication failures.
 For more info see: https://www.codrey.com/learn/rs-485-arduino-quick-primer/    especially about 'R7'
 
 Then you define your CMRI nodes (type = SMINI). The standard "SMINI" node has 24 inputs and 48 outputs
 In your loop code
    cmri.process();          // Is the main processing call to make CMRI work
    cmri.set_bit(x, true);   // Will set bit 'x' to true or false. Setting a bit here means it is an input in JMRI (a sensor)
    cmri.get_bit(y);         // Will retrieve the current value of bit 'y' as true/false. Getting a bit means it is a light in JMRI

 get_bit() and set_bit() work with different arrays so even if x = y they are not referring to the same bit     
 Using the nodes in JMRI you can define in Tools/Tables either Sensors (inputs) or Lights(outputs)
 In JMRI for each entry you define a hardware address. This maps to a specific SMINI node, and bit.
    Input address    1-  24 or output address    1-  48 are on CMRI_ADDR 0
    Input address 1000-1024 or output address 1000-1048 are on CMRI_ADDR 1
    Input address 2000-2024 or output address 2000-2048 are on CMRI_ADDR 2
    Input address 3000-3024 or output address 3000-3048 are on CMRI_ADDR 3
  etc

 ******************************************************************************/
#define CMRI_ADDR 0                 // Which SMINI node address are using for this MaxDuino
#define DE_PIN 2                    // Maxduino pin D2 is the RS-485 direction pin for half duplex flow control
Auto485 bus(DE_PIN, DE_PIN);        // We use the same pin for both DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);  // As shown allows explicit numbers. SMINI = 24 inputs, 48 outputs//

SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup() {
  mySoftwareSerial.begin(9600);  // for MP3 Player
  delay(10);
  Serial.begin(19200);  // for DEBUG console
  printConfig();        // display basic Info about program

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));
  Serial.println(" ");
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30

  // Starting the RS485 bus (line below) prevents normal serial debug console messages.
  bus.begin(19200, SERIAL_8N2);  // Start RS485 Comms at 19.2 Kbps, 8 data bits, no parity, 2 stop bits.
}

void loop() {
  static unsigned long timer = millis();
  cmri.process();  // The main processing node of cmri library

  if (millis() - timer > 10000) {              // Only if 10 seconds has expired check again.
    for (int track = 1; track < 7; track++) {  // loop through 7 possible CMRI bits
      if (cmri.get_bit(track)) {               // for the first bit found to be set
        cmri.set_bit(track, true);             // Set the bit in the sensor table for this track
        myDFPlayer.play(track);                // Play the appropriate track
        timer = millis();                      // reset the timer to allow playing at least 10 more seconds
      } else {
        cmri.set_bit(track, false);            // Clear the bit in the sensor table
      }
    }

    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
    }
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void printConfig(void) {
  /* 
    *******************
    ** printConfig() **
    *******************

   A subroutine that prints out basic information about the running program.
   Intended to be called once early in the startup code.  When connected to
   serial monitor this reveals what program is actually loaded on a given Arduino.

   Prior to invoking (typically on the first three lines of your sketch)
   the following three variables must be defined. Alter the numbers to suit of course.

  int VER_MAJ = 0;
  int VER_MIN = 0;
  int VER_BETA = 1;
*/
  Serial.println(">");
  // Source code
  Serial.println(F(">         File = " __FILE__));
  Serial.println(F(">        About = RS-485 and MP3 Demo using CMRI Address 0"));

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
  Serial.println(F(">    Copyright =  © Alan Lomax (MERG M8640) 2023"));
  Serial.println(">");
}