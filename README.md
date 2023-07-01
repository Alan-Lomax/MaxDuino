# MAXDuino KiCad files
These are the KiCad files for the MAXDuino PCB.

<Li> Arduino Nano R3
<li> MAX485 chip and two sets of terminals for daisy chaining
<li> on board NeoPixel
<Li> Robust on board DC power supply
<Li> DFRobot MP3 player with SD card and speaker terminals
<li> Lots of 3 pin headers and an I2C header
<Li> Opto Isolator for decoding DCC packets (Since Rev 1.1)

# Panel Version
The working PCB is sized 50mmm x 100 mm which means it can easily be panelized in sets of two.
There is a seperate folder containing the Panelized version of the Gerbers.
Note that all EDITS must be done in the 'single PCB version' and then be repanelized. 
Creating a new panel version is done by running a script.
It panelizes the single version into an array you specify (two rows, one collumn).  

## Status
<li>  MAXDuino development was as follows 

Rev 1.1 is the 'Production version' with the following fixes / enhancements:
<Li> (Schematic + PCB) Fix DCC decoder circuit.
<Li> (Schematic + PCB) Remove R3 - not required as pullup is included in U2
<Li> (Schematic + PCB) Change R4 from 4K7 to 470R to improve transient performance
<Li> (Schematic + PCB) Change R2 from 1K to 2K2 to reduce DCC current draw through optoisolator.
<Li> (Schematic + PCB) Change C5 from 220pF to .001uF  to improve transient performance
<Li> (Schematic + PCB) Add C8 0.1 uF as decoupling capacitor on U2
<Li> (PCB Only) Improved stencil markers for NeoPixel
<Li> (PCB Only) Improved stencil markers for DCC isolation
<Li> (Docs) Revise build instructions (esp the DCC decoding section)
<Li> (Docs) Added configurator to build instructions - decide what features you want and BoM updates accordingly

Rev 1.0 is the 'Pre production version' with the following fixes / enhancements:
<Li> (Schematic + PCB) Remove unnecessary jumper pins
<Li> (Schematic + PCB) Remove C6 (not needed)
<Li> (Schematic + PCB) Add terminals for 5VDC output or direct injection of 5VDC
<Li> (Schematic + PCB) Modify 3 pin headers construct from groups of 4 in different colours
<Li> (Schematic + PCB) Reorder the I2C pins to match OLED display order
<LI> (PCB Only) Make Power supply traces consistent (some were thinner than others)
<LI> (PCB Only) Move Vin_En jumper closer to the Arduino Vin pin
<Li> (PCB Only) Reduce footprint for electrolytic capacitors
<Li> (PCB Only) Improved stencil markers for 5VDC
<Li> (Docs) Revise build instructions (esp the new 5VDC PS option)

Rev 0.1 Was the first PCB manufactured. 
<Li> Mainly used as a platform for code development
<Li> and early issue resolution.
<Li> First pass build documentation
