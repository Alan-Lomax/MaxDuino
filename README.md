# MAXDuino KiCad files
These are the KiCad files and supporting documentation for the MaxDuino PCB.
## Key Features
<Li>Arduino Nano R3
<Li>MAX485 chip and two sets of terminals for daisy chaining
<Li>on board NeoPixel
<Li>Robust on board DC power supply
<Li>DFRobot MP3 player with SD card and speaker terminals
<Li>Lots of 3 pin headers and an I2C header
<Li>Opto Isolator for decoding DCC packets (Since Rev 1.1)

# Notes
In individual folders you will find the original KiCad project work plus the panelized gerbers.
The latter can be used to order your own PCB's from a fab shop of your choice.
Included are sample sketches for testing, along with detailed build instructions (in Excel)


# Panel Version
The working PCB is sized 50mmm x 100 mm which means it can easily be panelized in sets of two.
There is a seperate folder containing the Panelized version of the Gerbers.
Note that all EDITS must be done in the 'single PCB version' and then be repanelized. 
Creating a new panel version is done by running a script.
It panelizes the single version into an array you specify (two rows, one collumn).  

## Status of MAXDuino development 

### Rev 1.1 The current 'Production version' with the following fixes / enhancements:
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
