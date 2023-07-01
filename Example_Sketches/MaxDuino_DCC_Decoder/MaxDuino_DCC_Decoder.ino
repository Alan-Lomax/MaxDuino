int VER_MAJ = 0;
int VER_MIN = 0;
int VER_BETA = 1;
/******************************************************************************
   Demo code for testing the MAxDuino board
   Uses the on board optoisolator to decode DCC messages
   and output them to the serial monitor (A simple packet sniffer).
   
   Hardware assumptions:
      MAXDuiono PCB with DCC decoder circuitry in place. (inputs to pin 3)
      Arduino NANO
  Software Assumptions:
      Libraries Listed below are available
 ******************************************************************************/
//
#include <DCC_Decoder.h>
#include <Wire.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines and structures
//
//  On the Arduino NANO
//     Input D2 uses Int 0    << This was the default for this sketch
//     Input D3 uses Int 1    << This is the default for the MaxDuino board
#define kDCC_INTERRUPT 1

typedef struct
{
  int count;
  byte validBytes;
  byte data[6];
} DCCPacket;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The dcc decoder object and global data
//
int gPacketCount = 0;
int gIdlePacketCount = 0;
int gLongestPreamble = 0;

DCCPacket gPackets[25];

static unsigned long lastMillis = millis();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Packet handlers
//

// ALL packets are sent to the RawPacket handler. Returning true indicates that packet was handled. DCC library starts watching for
// next preamble. Returning false and library continue parsing packet and finds another handler to call.
boolean RawPacket_Handler(byte byteCount, byte* packetBytes) {
  // Bump global packet count
  ++gPacketCount;

  int thisPreamble = DCC.LastPreambleBitCount();
  if (thisPreamble > gLongestPreamble) {
    gLongestPreamble = thisPreamble;
  }

  // Walk table and look for a matching packet
  for (int i = 0; i < (int)(sizeof(gPackets) / sizeof(gPackets[0])); ++i) {
    if (gPackets[i].validBytes) {
      // Not an empty slot. Does this slot match this packet? If so, bump count.
      if (gPackets[i].validBytes == byteCount) {
        char isPacket = true;
        for (int j = 0; j < byteCount; j++) {
          if (gPackets[i].data[j] != packetBytes[j]) {
            isPacket = false;
            break;
          }
        }
        if (isPacket) {
          gPackets[i].count++;
          return false;
        }
      }
    } else {
      // Empty slot, just copy over data
      gPackets[i].count++;
      gPackets[i].validBytes = byteCount;
      for (int j = 0; j < byteCount; j++) {
        gPackets[i].data[j] = packetBytes[j];
      }
      return false;
    }
  }

  return false;
}

// Idle packets are sent here (unless handled in rawpacket handler).
void IdlePacket_Handler(byte byteCount, byte* packetBytes) {
  ++gIdlePacketCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
void setup() {
  Serial.begin(19200);
  printConfig();

  DCC.SetRawPacketHandler(RawPacket_Handler);
  DCC.SetIdlePacketHandler(IdlePacket_Handler);

  DCC.SetupMonitor(kDCC_INTERRUPT);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DumpAndResetTable() {
  char buffer60Bytes[60];

  Serial.print("Total Packet Count: ");
  Serial.println(gPacketCount, DEC);

  Serial.print("Idle Packet Count:  ");
  Serial.println(gIdlePacketCount, DEC);

  Serial.print("Longest Preamble:  ");
  Serial.println(gLongestPreamble, DEC);

  Serial.println("Count    Packet_Data");
  for (int i = 0; i < (int)(sizeof(gPackets) / sizeof(gPackets[0])); ++i) {
    if (gPackets[i].validBytes > 0) {
      Serial.print(gPackets[i].count, DEC);
      if (gPackets[i].count < 10) {
        Serial.print("        ");
      } else {
        if (gPackets[i].count < 100) {
          Serial.print("       ");
        } else {
          Serial.print("      ");
        }
      }
      Serial.println(DCC.MakePacketString(buffer60Bytes, gPackets[i].validBytes, &gPackets[i].data[0]));
    }
    gPackets[i].validBytes = 0;
    gPackets[i].count = 0;
  }
  Serial.println("============================================");

  gPacketCount = 0;
  gIdlePacketCount = 0;
  gLongestPreamble = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop
//
void loop() {
  DCC.loop();

  if (millis() - lastMillis > 2000) {
    DumpAndResetTable();
    lastMillis = millis();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  Serial.println(F(">        About = Arduino DCC Decoder/Sniffer"));

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
  Serial.println(F(">    Compiled for Maxduino by by Alan Lomax (MERG M8640) 2023"));
  Serial.println(">");
}