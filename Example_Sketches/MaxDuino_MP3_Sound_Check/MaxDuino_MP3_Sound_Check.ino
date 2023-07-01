int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 0;
/*
 * **********************************
 * ** For testing the MAXDuino PCB **
 * **********************************
     MAXDuino PCB versions R0 and R1

     Basic Test of 3 pin header and MP3 functionality.

     This code detects a sensor input on an Arduino pin (eg an IR sensor)
     The input is configured to use internal pullups, so it normally reads HIGH until the sensor module grounds the pin to activate things.
     When the pin is grounded a command "play next" is sent to the DFPlayer. (with multiple tracks on SD card they will play in round robin fashion)
     Serial communication are on pins D10 and D11 (Rx, Tx) which is also typical of code found on the internet.

     Created 2022-12-23
     By Alan Lomax


  0.1.0 Maxduino R1. Added diagnostic information to console on startup - always 19200
*/
//////////////////////////////////////////
// includes
#include <DFPlayerMini_Fast.h>

#if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX
#endif

#include "dfplayer_error_code_definitions.h"

// change the following iine to test one input pin at a time
const int SensorPin = 9;  // A0, A1, A2, A3, A4, A5, 4 ,5, 6, 7, 8 9  Note: D13 is uesd as LED output, and A6/A7 lack digital input capability
// constants
const int ledPin = 13;       // the number of the LED pin. The on board LED 13 can be used to let us know what is going on
const int RxPin = 10;        // the number of the Rx (Receive) pin coming back from the MP3 player.
const int TxPin = 11;        // the number of the Tx (Transmit) pin going to the MP3 player.
const int BusyPin = 12;      // the number of the Busy pin coming back from the MP3 player.
const int shortDelay = 100;  // The MP3 player takes time to decode and process conmmands.
                             // It is possible to overwrite a command before it is acted on.
                             // This delay is used after every command is sent to avoid crushing it.

// variables
bool ACTIVATED = false;    // Different Sensors have different idle state. Will adjust during setup further down.
bool SensorState = false;  // Will hold the current sensor state.
bool isPlaying = false;    // Status of MP3 player. Assumed off to start

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
  pinMode(SensorPin, INPUT);    // sets the Sensor Pin as input
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

  ACTIVATED = !digitalRead(SensorPin);  // On startup assume the sensor is 'idle' so set Activated status to be the opposite.

  // ***********************************************************************************************************
  // ** The following line may need adjusting! (Individual MP3 modules and speakers will respond differently) **
  // ** Values from 5 (quiet) to 20 (moderately loud) have been found to work well.                           **
  // ***********************************************************************************************************
  delay(500);            // pause to let things initialize after power on. (not same as pressing reset)
  myDFPlayer.volume(4);  // Set volume value. From 0 to 30. (4 = quiet, 10 = medium, > 20 = very loud)
  delay(shortDelay);     // pause to let things initialize after power on.
  delay(shortDelay);     // pause to let things initialize after power on.

  Serial.println();
  Serial.println(F("Volume changed to 4"));
  printState();  // print status report to Serial monitor
  Serial.println();
}

void loop() {
  // Use one of the following two lines
  // isPlaying = digitalRead(BusyPin);  // Get 'live' play status from player (using BusyPin)
  isPlaying = myDFPlayer.isPlaying();  // Get 'live' play status from player. By Software command (slower)

  myDFPlayer.volume(4);  //Set volume value. From 0 to 30.

  SensorState = digitalRead(SensorPin);  // Get the sensor status
  if (SensorState == ACTIVATED) {
    digitalWrite(ledPin, true);  // sensor is activated so turn our LED on (so we know that arduino knows!)
    Serial.println(F("Inside Loop with Sensor Activated"));
    printState();             // print status report to Serial monitor
    if (!isPlaying) {         // Only play a new track if current track is finished.
      myDFPlayer.playNext();  // Play next mp3;
      delay(shortDelay);      // pause to let it process that.
    }
  } else {
    digitalWrite(ledPin, false);  // sensor is NOT activated turn our LED off (so we know that arduino knows!)
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
  Serial.println(F(">    Copyright =  © Alan Lomax (MERG M8640) 2023"));
  Serial.println(">");
}


void printState() {
  delay(shortDelay);                   // pause to let it process any previous commands
  Serial.print(F("Sensor Pin is: "));  // current sensor pin definition
  Serial.println(SensorPin);
  Serial.print(F("Sensor Pin State: "));  // sensor pin state
  Serial.println(digitalRead(SensorPin));
  Serial.print(F("Busy Pin State: "));
  Serial.println(digitalRead(BusyPin));  // Get 'live' play status from player. (By Busy Pin hardware )
  Serial.print(F("isPlaying State: "));
  Serial.println(myDFPlayer.isPlaying());  // Get 'live' play status from player. (By Software)
  delay(shortDelay);                       // pause to let it process that.
  delay(shortDelay);                       // pause to let it process that.
  Serial.print(F("Volume Setting: "));
  Serial.println(myDFPlayer.currentVolume());
  delay(shortDelay);  // pause to let it process that.
  delay(shortDelay);  // pause to let it process that.
  Serial.print(F("Current Track: "));
  Serial.println(myDFPlayer.currentSdTrack());
  delay(shortDelay);  // pause to let it process that.
  delay(shortDelay);  // pause to let it process that.
  Serial.println();
}