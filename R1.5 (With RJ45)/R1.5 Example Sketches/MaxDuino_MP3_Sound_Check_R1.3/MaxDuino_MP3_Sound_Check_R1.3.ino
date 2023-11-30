int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 3;
/*
 * **********************************
 * ** For testing the MAXDuino PCB **
 * **********************************
  Basic Test of MP3 functionality.
  Sequentially plays a few seconds of sound clips from the SD card

  Created Oct 14 2023
  By Alan Lomax
*/
//////////////////////////////////////////
// includes
#include <DFPlayerMini_Fast.h>

// MaxDuino uses pins 10 and 11 for Software Serial comms to the MP3 player module.
#if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX
#endif

#include "dfplayer_error_code_definitions.h"

// constants
const int ledPin = 13;   // the number of the LED pin. The on board LED 13 can be used to let us know what is going on
const int RxPin = 10;    // the number of the Rx (Receive) pin coming back from the MP3 player.
const int TxPin = 11;    // the number of the Tx (Transmit) pin going to the MP3 player.
const int BusyPin = 12;  // the number of the Busy pin coming back from the MP3 player.
// ***********************************************************************************************************
// ** The following line may need adjusting! (Individual MP3 modules and speakers will respond differently) **
// ** Values from 5 (quiet) to 20 (moderately loud) have been found to work well.                           **
// ***********************************************************************************************************
const int volume = 9;        // the volume value. From 0 to 30. (4 = quiet, 10 = medium, > 20 = very loud)
const int shortDelay = 100;  // The MP3 player takes time to decode and process conmmands.
                             // It is possible to overwrite a command before it is acted on by the mpdule.
                             // This delay is used after every command is sent to avoid crushing it.

// variables
bool isPlaying = false;  // Status of MP3 player. Assumed off to start

// define a software serial channel
SoftwareSerial myMP3Serial(RxPin, TxPin);  // These pins are used as RX, TX respectively. (Rx at the Arduino connects to Tx on the MP3 player.)

// define the player object
DFPlayerMini_Fast myDFPlayer;

// declare the routine for printing MP3 diagnositics and error messages
void printDetail(uint8_t type, int value);

void setup() {
  Serial.begin(19200);  // for debug console
  printConfig();        // output basic info

  pinMode(ledPin, OUTPUT);      // sets the LED pin as output
  pinMode(BusyPin, INPUT);      // sets the Busy Pin from the MP3 player as input
  digitalWrite(ledPin, false);  // sets the LED off to start with

  myMP3Serial.begin(9600);  // for MP3 Player
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(myMP3Serial, false)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }

  Serial.println(F("DFPlayer Mini online."));
  delay(1000);  // Long pause to let things initialize after power on.

  myDFPlayer.volume(volume);
  delay(shortDelay);  // pause to let things initialize after power on.
  Serial.println();
  Serial.println(F("Volume changed"));
}

void loop() {
  // Use one of the following two lines
  isPlaying = !digitalRead(BusyPin);      // Get 'live' play status from player (using BusyPin). Note it is inverted. Tested ok Oct 14 2023
  // isPlaying = myDFPlayer.isPlaying();  // Get 'live' play status from player. By Software command (slower) Tested ok Oct 14 2023

  if (!isPlaying) {  // Only play a new track if we are not playing something now.
    digitalWrite(ledPin, false);
    Serial.println(F("Playing is Idle"));
    myDFPlayer.playNext();  // Play next mp3;
    delay(shortDelay);      // pause to let it process that.
    Serial.println(F(" - Play next track command sent"));
  } else {
    digitalWrite(ledPin, true);  // LED to let us know
    Serial.println(F("  - Player is busy."));
    delay(6000);                 // As we are playing something - wait at least 6 more seconds
  }
}

// This is the Error Reporting Routine
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
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
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
  Serial.println(F(">    Alan Lomax (MERG M8640) Oct 2023"));
  Serial.println(">");
}