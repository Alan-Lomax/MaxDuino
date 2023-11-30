# MaxDuino
Is a small PCB carrier board that acts as a host for an Arduino NANO.
The PCB provides a multitude of supporting features for the NANO.
It is targeted for use as a model railway animation tool (see feature list)
Because of its flexibility it can be used in many different scenarios.

## Key Features
<Li>Arduino Nano R3 is fully utilized.
<Li>On board NeoPixel for flexible diagnostic use and status reporting.
<Li>Robust on board DC power supply (5V DC, 9-15V DC/AC, or DCC)
<Li>DFRobot MP3 player with SD card and speaker terminals
<Li>Lots of 3 pin headers and an I2C header
<Li>Opto Isolator for decoding DCC packets
<Li>A RJ45 jack is used to connect 12VDC power and RS485 signals between boards
<li>Two sets of jacks are provided for easy daisy chaining
<Li>The MAX485 chip has seperate DE/RE pins allowing ICSP of the Arduino. 
<Li>Sample sketches for testing, and to accelerate your own development.
<li>Detailed build instructions are provided in Excel.

## Status of MaxDuino development 
### Rev 1.5 Is The Latest 
<Li>This version is electrically the same as Version 1.4) (no major functionality difference)</Li>
<li>The previous screw terminals for RS485 have been replaced with RJ45 jacks.
<li>Of the 8 pins available two are used for RS485, and 4 are used for 12VDC (doubled up). The remaining two are for future use.
<Li>The power supply section received a major redesign - replacing a round bridge diode with 4 discrete diodes
<Li>Furthermore 3 additional diodes were used to drop the voltage into the L7805 voltage regulator
<Li>A single diode has been used to prevent backfeeding power onto the network cable 
<li>The previous screw terminals for a speaker are replaced with PCB holes for mounting a 2 pin header or direct soldering.
<Li><B>All testing passed successfully.</B> 
<Li>This includes remote power and RS485 communications via a single standard network patch cable</Li>

## Known Issues
<Li>The power supply redesign moved the parts around such that the heat sink does not fit without modification. 
This is offset however as the additional diodes mean heat dissipation is not as much a concern. 
<Li>On the 5 Volt screw terminals the stencilling for plus and minus was inadvertently ommitted. 

### Rev 1.4
<Li> This version is electrically the same as Version 1.3)</Li>
<Li> Minor stencilling improvements and corrections have been addressed (from version 1.3)</Li>
<Li> <B>All functional testing passed successfully.</B></Li>
<Li> Updated test sketches provided. Reflects new pin assignments, simplified and compartmentalized the test scope.</Li>

### Rev 1.3 and Older Have been removed from GitHub

## MaxDuino KiCad files including GERBERS.
The KiCad files are essentially 'source code' and include the sc schematic and the Printed Circuit Board.
The Gerber files are the 'compiled version' and are whjat gets submitted for manufacture at the fabrication facility of your choice.

# The Panel Version
There is a seperate folder containing the Panelized version of the Gerbers.

The MaxDuino PCB is sized 50mmm x 100 mm which means it can easily be panelized in sets of two.
This makes the panelized Maxduino fit on a single 100mm x 100mm PCB which is the normal 'minimum size' for billing.
The short version - you get twice as many MaxDuinos at no extra cost.

# Be Aware
Normally all EDITS should be done in the 'single PCB version' and then it be repanelized.
To repanleize you start a new Kicad project (no contents) and run the add in to panelize.
You will specfiy which other KiCad design you want to panelize and the number of rows (2) and collumns (1)
The only manual step needed is to add a horizontal line on the edge cut layer.
This will be a 'Vee-Cut' and allows separating the two PCBs easily without cutting, sawing or scoring.



