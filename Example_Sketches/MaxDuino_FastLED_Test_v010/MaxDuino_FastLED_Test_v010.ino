int VER_MAJ = 0;
int VER_MIN = 1;
int VER_BETA = 0;
/*
  rev 0 - basic code
  0.0.1 debugging down to lightning
  0.0.2 working on sunrise
  0.0.3 sunrise algo ok. color pallete picked.
  0.0.4 changed to enumerations, added a different color pallete for sunset
  0.0.5 tweaking of sunset
  0.0.6 Added second physical strip of LED's. Improved sunrise and sunset (esp start and end conditions = daylight and nightime) 
  0.0.7 - not used
  0.0.8 Moved testing over to MAXDuino hardware (was previously on MergDuino)
  0.1.0 Maxduino R1. Added diagnostic information to console on startup - always 19200

  on deck - Develop and test a Partly Cloudy daytime
          - add twinkling stars to nightime (random brief full on as in lightening only a single pixel)
          - Fullday routine - wrapping each sequence in a state machine script
          - conversion to an object class


   Main lighting sketch including code from Ian Clarke's LMRG Master Lighting sketch
   December 2022 - Alan Lomax, MERG Arduino SIG, The Platelayers, The Buffers
   October 2020 - Ian Clarke, London Model Railroad Group

  This sketch combines several lighting sequences for simple-array Neopixels into one sketch, toggled by a
  momentary-on switch connected between ground and a signal pin defined below. Several effects allow for colour or brightness
  control using a potentiometer connected to ground and +5VDC on the outside and A0 on the wipers.

  Several strips of Neopixels can be connected
*/
#include <Led3.h>
#include <Button.h>
#include "FastLED.h"
#include <EEPROM.h>  // Allows Arduino board to retain last selected sequence

// Define constants here
#define NUM_LEDS 10    // Your Neopixel strip length (was 96)
#define LedStrip0 A0   // Pin for first led strip
#define LedStrip1 A1   // Pin for second led strip
#define shortDelay 50  // 50ms delay

// #define LED_D4 4               // discrete external LED
#define LED_D13 13             // discrete external LED
const bool Activated = false;  // Because the input is declared with input pullup
const byte SensPin1 = A2;      // Use this input for sensor input.
enum ToD { en_OFF,
           en_WHITE,
           en_COLOR_WHEEL,
           en_LIGHTNING,
           en_SUNLIGHT,
           en_NIGHTIME,
           en_SUNRISE,
           en_DAYLIGHT,
           en_SUNSET,
           en_FULLDAY };
ToD selectedEffect = en_OFF;  // Effects enumeration starts in the off position

// Define objects here
Button SENSOR1(SensPin1, 5);  // Defining object outside of setup makes it 'global' and accessable from any part of the program.
Led3 Heartbeat(LED_D13);      // Indicate Arduino is running

// Define global variables here
boolean currentBtn = true;  // Define variable for current button state so we can act on it without reading it several times.
boolean prevBtn = true;     // Define variable for previous button state so we can act on a change only.

unsigned int dimmer = 1;                  // Used by lightning sequence
unsigned long lightning_interval = 8000;  // Minimum Time between lightning Strike1s in ms
unsigned long NextDue = millis();         // NextDue time for next special effect (eg lightning Strike1).
long delta = 0;                           // time difference to calculate if we need another lightening Strike. Note it can be negative.
boolean Strike1 = true;                   // Is this the first (primary) lightning Strike? Flase indicates a reflash of one in progress.
int EndFlashCount = 0;                    // Will be set to a random number for target number of flashes in a given lightning Strike1
int FlashCounter = 0;                     // The current counter for the number of flashes in a given lightning Strike1
boolean firstRun = true;                  // flag to control code that runs once inside the loop

static const uint8_t sunriseLength = 5;  // total sunrise length, in minutes
static const float interval = ((float)(sunriseLength * 60) / 256) * 1000;
uint8_t heatIndex = 5;  // start out at 5 (night)

CRGB rawleds[NUM_LEDS];  // Sets up LEDs in blocks of 1 or more so fill_solid can be applied selectively
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet leds0(leds(0, 2));  // these are substrings of the overall array.
CRGBSet leds1(leds(2, 2));
CRGBSet leds2(leds(4, 2));
CRGBSet leds3(leds(6, 2));
CRGBSet leds4(leds(8, 2));

struct CRGB* ledarray[] = { leds0, leds1, leds2, leds3, leds4 };


DEFINE_GRADIENT_PALETTE(SunRiseColours_gp){
  // Color palette for night to day. This pallette from:
  // a custom pallett created using poweperpoint gradient stops
  // and an excel tool to look up RGB values for the various stops
  //# COLOR_MODEL = RGB
  0, 0, 0, 54,
  105, 6, 36, 78,
  125, 128, 26, 0,
  135, 172, 155, 0,
  145, 130, 130, 20,
  150, 140, 140, 60,
  155, 90, 100, 100,
  160, 60, 100, 155,
  255, 60, 100, 160
};

DEFINE_GRADIENT_PALETTE(SunSetColours_gp){
  // Custom color palette for day to night
  // Note percentages are inverted. This routine counts down.
  0, 0, 0, 54,        // 100% nightime
  70, 4, 4, 78,       //  85% inky blue
  95, 54, 10, 0,      //  65% Dark red
  145, 80, 20, 0,     //  55% red sun
  135, 100, 44, 0,    //  52% orange sun
  160, 100, 100, 40,  //  50% yellow sun
  175, 50, 90, 155,   //  45% slightly dimmer
  255, 60, 100, 160   //   0% daylight
};
CRGBPalette16 mySunSetPal = SunSetColours_gp;
CRGBPalette16 mySunRisePal = SunRiseColours_gp;

void setup() {
  Serial.begin(19200);  // for debug console
  printConfig();        // output basic info

  // pinMode(LED_D4, OUTPUT);
  Heartbeat.setMode(Blink);
  Heartbeat.configBlink(5, 500);  // 1ms = a dim flash, 10ms full brightness flash

  delay(3000);  // 3-second power-up safety delay
  FastLED.addLeds<WS2812B, LedStrip0, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LedStrip1, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  EEPROM.get(0, selectedEffect);
  Serial.print("EEPromFx: ");
  printCurrentMode();  // put current mode on debug monitor
  Serial.print("StartFx: ");
  printCurrentMode();  // put current mode on debug monitor
  Serial.println("");
}

// the loop function runs over and over again forever
void loop() {
  SENSOR1.update();  // See if anything has changed
  Heartbeat.update();
  currentBtn = SENSOR1.state();  // read what it is (true or false)
  if (currentBtn == prevBtn) {
    // If it was the same as last time through we dont need to do anything.
  } else {                 // If we are here the button is different than last time
    prevBtn = currentBtn;  // make the previous button state the same as the current - so we dont keep executing this block
    if (currentBtn == Activated) {
      // digitalWrite(LED_D4, HIGH);  // Activate external discrete LED so we know this logic test is working.
      // These are the enums {en_OFF, en_WHITE, en_COLOR_WHEEL, en_LIGHTNING, en_SUNLIGHT, en_NIGHTIME, en_SUNRISE, en_DAYLIGHT, en_SUNSET, enFULLDAY}
      // on a button push the following code cycles us through the effects in order. The last one returns to the top one.
      switch (selectedEffect) {
        case en_OFF:
          {
            selectedEffect = en_WHITE;
            break;
          }
        case en_WHITE:
          {
            selectedEffect = en_COLOR_WHEEL;
            break;
          }
        case en_COLOR_WHEEL:
          {
            selectedEffect = en_LIGHTNING;
            break;
          }
        case en_LIGHTNING:
          {
            selectedEffect = en_SUNLIGHT;
            break;
          }
        case en_SUNLIGHT:
          {
            selectedEffect = en_NIGHTIME;
            break;
          }
        case en_NIGHTIME:
          {
            selectedEffect = en_SUNRISE;
            break;
          }
        case en_SUNRISE:
          {
            selectedEffect = en_DAYLIGHT;
            break;
          }
        case en_DAYLIGHT:
          {
            selectedEffect = en_SUNSET;
            break;
          }
        case en_SUNSET:
          {
            selectedEffect = en_FULLDAY;
            break;
          }
        case en_FULLDAY:
          {
            selectedEffect = en_OFF;
            break;
          }
      }
      printState();
      EEPROM.put(0, selectedEffect);  // Save it in case of power outage
      firstRun = true;                // Since we changed modes enforce starting as though a first run in that mode
    } else {
      // digitalWrite(LED_D4, LOW);  // We must have let go of button so now deactivate external discrete LED
    }
  }
  //
  // Now do whatever the selected effect calls for
  //
  switch (selectedEffect) {
    case en_OFF:
      {
        LEDsOff();
        break;
      }
    case en_WHITE:
      {
        BrightWhite();
        break;
      }
    case en_COLOR_WHEEL:
      {
        ChangeColour();
        break;
      }
    case en_LIGHTNING:
      {
        delta = NextDue - millis();  // NextDue is a future time for lightning to Strike, millis is current time,
        if (delta < 0) {             // Once we go negative it is time for lightning to Strike
          lightning();               // The routine will calculate a new NextDue for us to use next time
        }
        break;
      }
    case en_SUNLIGHT:
      {
        SunLight();
        break;
      }
    case en_NIGHTIME:
      {
        nighttime();
        break;
      }
    case en_SUNRISE:
      {
        sunrise();
        break;
      }
    case en_DAYLIGHT:
      {
        Daylight();
        break;
      }
    case en_SUNSET:
      {
        sunset();
        break;
      }
    case en_FULLDAY:
      {
        //   need to wrap an index around executing these in sequence
        //
        //        nighttime()
        //        sunrise();
        //        Daylight();
        //        sunset();
        break;
      }
  }
}

// *************************
// ** LEDEffect Functions **
// *************************

void LEDsOff() {
  fill_solid(leds, NUM_LEDS, CHSV(0, 0, 0));
  FastLED.show();
}

void BrightWhite() {
  fill_solid(leds, NUM_LEDS, CHSV(0, 0, 240));
  FastLED.show();
}

void Daylight() {
  //  81,160,170
  FastLED.setBrightness(80);  // Sets brightness to Daylight levels (80 - 120 seems good)
  fill_solid(leds, NUM_LEDS, CRGB(60, 100, 160));
  FastLED.show();
}

void ChangeColour() {
  int sensor_output = 128;  // Orignially Ian Used a pot to change value and allow manual adjustment of colour. I jigged it for now. Brightness dialed down to 150.
  float MyColour = sensor_output / 4;
  fill_solid(leds, NUM_LEDS, CHSV(MyColour, 255, 150));
  FastLED.show();
}

void SunLight() {
  FastLED.setBrightness(120);  // Sets brightness to bright sunlight levels (120 seems good)
  fill_solid(leds, NUM_LEDS, CHSV(32, 255, 150));
  FastLED.show();
}

void sunrise() {
  if (firstRun) {
    FastLED.setBrightness(80);  // Sets brightness to medium low (80 seems good)
    heatIndex = 5;              // for sunrise we start with a low heatindex
    firstRun = false;
  }
  EVERY_N_MILLISECONDS(500) {  // and slowly increase the heat
    CRGB color_0 = ColorFromPalette(mySunRisePal, heatIndex + 5, heatIndex + 5, LINEARBLEND);
    CRGB color_1 = ColorFromPalette(mySunRisePal, heatIndex + 2, heatIndex + 2, LINEARBLEND);
    CRGB color_2 = ColorFromPalette(mySunRisePal, heatIndex, heatIndex, LINEARBLEND);
    CRGB color_3 = ColorFromPalette(mySunRisePal, heatIndex - 2, heatIndex - 2, LINEARBLEND);
    CRGB color_4 = ColorFromPalette(mySunRisePal, heatIndex - 5, heatIndex - 5, LINEARBLEND);

    // each of our test LED strips is only two LED's long in a total of 10 LEDs
    fill_solid(ledarray[0], 2, color_0);
    fill_solid(ledarray[1], 2, color_1);
    fill_solid(ledarray[2], 2, color_2);
    fill_solid(ledarray[3], 2, color_3);
    fill_solid(ledarray[4], 2, color_4);
    FastLED.show();
    heatIndex++;
    // Serial.println(heatIndex);
  }
  // stop incrementing at 250, we don't want to overflow back to 0
  if (heatIndex > 250) {           // allow for offsets
    selectedEffect = en_DAYLIGHT;  // Go to daylight mode
    firstRun = true;     // reset for next first run
  }
}

void sunset() {
  if (firstRun) {
    FastLED.setBrightness(120);  // Sets brightness to medium low (120 seems good)
    heatIndex = 252;             // for sunset we have a different pallett
    firstRun = false;
  }
  // slowly decrease the heat
  EVERY_N_MILLISECONDS(500) {
    CRGB color_0 = ColorFromPalette(mySunSetPal, heatIndex - 3, heatIndex - 3, LINEARBLEND);
    CRGB color_1 = ColorFromPalette(mySunSetPal, heatIndex - 2, heatIndex - 2, LINEARBLEND);
    CRGB color_2 = ColorFromPalette(mySunSetPal, heatIndex, heatIndex, LINEARBLEND);
    CRGB color_3 = ColorFromPalette(mySunSetPal, heatIndex + 2, heatIndex + 2, LINEARBLEND);
    CRGB color_4 = ColorFromPalette(mySunSetPal, heatIndex + 3, heatIndex + 3, LINEARBLEND);
    // each of our test LED strips is only two LED's long in a total of 10 LEDs
    fill_solid(ledarray[0], 2, color_0);
    fill_solid(ledarray[1], 2, color_1);
    fill_solid(ledarray[2], 2, color_2);
    fill_solid(ledarray[3], 2, color_3);
    fill_solid(ledarray[4], 2, color_4);
    FastLED.show();
    heatIndex--;
    // Serial.println(heatIndex);
  }
  if (heatIndex < 22) {            // Not completely understood but at counts less than 20 LEDS go off instead of showing dark blue as expected.
    selectedEffect = en_NIGHTIME;  // Go to night time mode
    Serial.print("NewFx: ");
    printCurrentMode();  // put the current mode on debug monitor
    firstRun = true;     // reset for first run
  }
}

void nighttime() {
  FastLED.setBrightness(20);                   // Sets brightness to nighttime levels (30 - 60 seems good)
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 64));  // Red confirmed , Green Confirmed, Blue 64 is a good nightime light.
  FastLED.show();
}

void lightning() {
  // The first "flash" in a bolt of lightning is the "leader." The leader
  // is usually duller and has a longer delay until the next flash. Subsequent
  // flashes, the "strokes," are brighter and happen at shorter intervals.
  //
  // This code needs modifying to be non-blocking.
  //
  if (Strike1 == true) {                 // this is the first Strike1
    FastLED.showColor(CHSV(255, 0, 0));  // bright white
    FlashCounter = 1;
    EndFlashCount = random8(3, 10);                                     // number of subsequent Strikes is calculated during first Strike only
    dimmer = 6;                                                         // the leader brightness scaled down by factor of 6
    fill_solid(ledarray[1], random(3, 10), CHSV(40, 0, 255 / dimmer));  // Flashes one part of array
    FastLED.show();
    delay(random8(4, 10));                                              // each flash only lasts 4-10 milliseconds
    LEDsOff();                                                          // use existing code to shutoff
    NextDue = millis() + random8(10, 50);                               // each flash has a gap of 10-50 milliseconds
    Strike1 = false;                                                    // we are now past the first strike
  } else {                                                              // a subsequent Strike after the first
    dimmer = random8(1, 3);                                             // return strokes are brighter than the leader, scaled down by factor of 1-3
    fill_solid(ledarray[1], random(3, 10), CHSV(40, 0, 255 / dimmer));  // Flashes one part of array
    FastLED.show();
    delay(random8(4, 10));                 // each flash only lasts 4-10 milliseconds
    LEDsOff();                             // use existing code to shutoff
    NextDue = millis() + random8(10, 50);  // and each flash has a gap of 10-50 milliseconds
    FlashCounter++;                        // add to the flash counter
    if (FlashCounter > EndFlashCount) {    // Only if we are all done
      Strike1 = true;                      // reset to Strike1 for next time
      FlashCounter = 0;                    // and clear the flash counter
      int newGap = (random16(lightning_interval, 2 * lightning_interval));
      NextDue = millis() + newGap;  // calculate a future NextDue for a new first strike
    }
  }
}

void printCurrentMode() {
  switch (selectedEffect) {
    case en_OFF:
      {
        Serial.println("en_OFF");
        break;
      }
    case en_WHITE:
      {
        Serial.println("en_WHITE");
        break;
      }
    case en_COLOR_WHEEL:
      {
        Serial.println("en_COLOR_WHEEL");
        break;
      }
    case en_LIGHTNING:
      {
        Serial.println("en_LIGHTNING");
        break;
      }
    case en_SUNLIGHT:
      {
        Serial.println("en_SUNLIGHT");
        break;
      }
    case en_NIGHTIME:
      {
        Serial.println("en_NIGHTIME");
        break;
      }
    case en_SUNRISE:
      {
        Serial.println("en_SUNRISE");
        break;
      }
    case en_DAYLIGHT:
      {
        Serial.println("en_DAYLIGHT");
        break;
      }
    case en_SUNSET:
      {
        Serial.println("en_SUNSET");
        break;
      }
    case en_FULLDAY:
      {
        Serial.println("en_FULLDAY");
        break;
      }
    default:
      {
        Serial.println("Unknown enumeration.");
        Serial.println("Setting to default (OFF)");
        selectedEffect = en_OFF;        // set our enumeration
        EEPROM.put(0, selectedEffect);  // Save it in case of power outage
        Serial.println("NewFx: OFF");
        firstRun = true;  // Since we changed modes enforce starting as though a first run in that mode
        break;
      }
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
  /* 
    ********************
    ** printState()   **
    ********************

   A subroutine that prints out the state of key variables.
   Intended to be called as needed when you want to know what is going on. 
   When connected to the serial monitor this reveals what your program is doing.
   This code needs to be modified per sketch but in your main sketch
   yuo only need to speinlle around calls to prinState() to make it happen
   with consitent variables and formatting.
*/
  delay(shortDelay);               // pause to let it process any previous commands
  Serial.print(F(" millis(): "));  //
  Serial.println(millis());
  // add more text:variable pairs as needed
  Serial.print("    NewFx: ");
  printCurrentMode();  // put the current mode on debug monitor
  Serial.print("   NumLED: ");
  Serial.println(NUM_LEDS);
  Serial.print("LedStrip0: ");
  Serial.println(LedStrip0);
  Serial.print("LedStrip1: ");
  Serial.println(LedStrip1);
  Serial.println();
}
