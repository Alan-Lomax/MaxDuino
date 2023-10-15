# MAXDuino
Is a small PCB carrier board that acts as a host for an Arduino NANO to plug into.
The PCB provides a multitude of supporting features for the NANO and is targeted for
use as a model railway animation tool (see feature list below)
Because of its flexibility it can be used in many different scenarios.

## MAXDuino KiCad files
These are the KiCad files and supporting documentation for the MaxDuino PCB.

## Key Features
<Li>Arduino Nano R3
<Li>MAX485 chip and two sets of terminals for RS485 daisy chaining (Seperate DE/RE pins)
<Li>on board NeoPixel for flexible diagnostic use
<Li>Robust on board DC power supply (Input can be any one of 5V DC, 9-15V DC/AC, or DCC)
<Li>DFRobot MP3 player with SD card and speaker terminals
<Li>Lots of 3 pin headers and an I2C header
<Li>Opto Isolator for decoding DCC packets

## Notes
In individual folders you will find the original KiCad project work plus the panelized gerbers.
The latter can be used to order your own PCB's from a fab shop of your choice.
Included are sample sketches for testing, along with detailed build instructions (in Excel)

## Status of MAXDuino development 
### Rev 1.4 Is The latest 
<Li> This version is electrically the same as Version 1.3)</Li>
<Li> <B>All functional testing passed successfully.</B></Li>
<Li> Minor stencilling improvements and corrections have been addressed (from version 1.3)</Li>
<Li> Updated test sketches provided. Reflects new pin assignments, simplified and compartmentalized the test scope.</Li>

### Rev 1.3 
<Li> (Schematic + PCB) Moved RE pin to pin D5 and returned DCC decoding to D3.</Li>
<Li> All testing has passed using the test sketches provided.</Li>
<Li> Prepared final draft of MERG article on this basis - during proofing minor stencil layer issues were noted.</Li>

### Rev 1.2 Minor Fixes and enhancements:
<Li> (Schematic + PCB) Used seperate pins for DE and RE on RS485 communication chip to enable ICSP.</Li>
<Li> (Schematic + PCB) Dasiy chained on board NeoPixel to the external pin header so thta both can be used without a duplication on index 0 of fastled array.</Li>
<Li> (Schematic + PCB) Added bypass to on board NeoPixel so that D4 could be connected directly to header pins.</Li>
<Li> (Schematic + PCB) <B>Regression</B> - Moving DE/RE pin assignments caused DCC decoding to be assigned a pin that does not support the necessary interrupts.</Li>


### Rev 1.1 History of Fixes fixes / enhancements:
<Li> (Schematic + PCB) Fix DCC decoder circuit.</Li>
<Li> (Schematic + PCB) Remove R3 - not required as pullup is included in U2</Li>
<Li> (Schematic + PCB) Change R4 from 4K7 to 470R to improve transient performance</Li>
<Li> (Schematic + PCB) Change R2 from 1K to 2K2 to reduce DCC current draw through optoisolator.</Li>
<Li> (Schematic + PCB) Change C5 from 220pF to .001uF  to improve transient performance</Li>
<Li> (Schematic + PCB) Add C8 0.1 uF as decoupling capacitor on U2</Li>
<Li> (PCB Only) Improved stencil markers for NeoPixel</Li>
<Li> (PCB Only) Improved stencil markers for DCC isolation</Li>
<Li> (Docs) Revise build instructions (esp the DCC decoding section)</Li>
<Li> (Docs) Added configurator to build instructions - decide what features you want and BoM updates accordingly</Li>
<Li> (Docs) Added test sketch details. For each test what hardware is needed and how to set it up in order to repeat the test case.</Li>

### Rev 1.0 A 'Pre production version' with the following fixes / enhancements:
<Li> (Schematic + PCB) Remove unnecessary jumper pins</Li>
<Li> (Schematic + PCB) Remove C6 (not needed)</Li>
<Li> (Schematic + PCB) Add terminals for 5VDC output or direct injection of 5VDC</Li>
<Li> (Schematic + PCB) Modify 3 pin headers construct from groups of 4 in different colours</Li>
<Li> (Schematic + PCB) Reorder the I2C pins to match OLED display order</Li>
<LI> (PCB Only) Make Power supply traces consistent (some were thinner than others)</Li>
<LI> (PCB Only) Move Vin_En jumper closer to the Arduino Vin pin</Li>
<Li> (PCB Only) Reduce footprint for electrolytic capacitors</Li>
<Li> (PCB Only) Improved stencil markers for 5VDC</Li>
<Li> (Docs) Revise build instructions (esp the new 5VDC PS option)</Li>
<Li> Note: DCC decoding failed to work.</Li>

### Rev 0.1 Early Development 
<Li> the first PCB manufactured.</Li> 
<Li> Mainly used as a platform for code development and early issue resolution.</Li>
<Li> Used to prepare first pass build documentation</Li>

# The Panel Version
The working PCB is sized 50mmm x 100 mm which means it can easily be panelized in sets of two.
There is a seperate folder containing the Panelized version of the Gerbers.
Note that all EDITS must be done in the 'single PCB version' and then be repanelized. 
Creating a new panel version is done by running a script.
It panelizes the single version into an array you specify (two rows, one collumn).  
