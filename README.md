# Eeprom 

This provides a program that allows reprogramming EEPROM's with a raspi via spi.
This package includes the modfied C++ lib wrapper to linux user space spidev
see https://github.com/milekium/spidev-lib/
This was based on work done by Stefan Mavrodiev 
for the pyA20 python library https://pypi.python.org/pypi/pyA20

## Connecting

For me it was possible to program the eeprom in place.

I used the following connections AT25F1024:

<pre>
RASPI                          Eeprom
 1 3.3V ---------------------- 8 VCC
 6 GND  ---------------------- 4 GND
19 MOSI ---------------------- 5 SI
21 MISO ---------------------- 2 SO
23 SCLK ---------------------- 6 SCK
24 CE0 ----------------------- 1 CS
</pre>

Requires the config.txt setting:

<pre>
dtparam=spi=on
</pre>

## Warning

It is not a application, please look into the source, you will most likely need to adapt it (check the chip manual).

If you want other user than root have access to spidev device you may probably need to add a file with the following udev rules in your /etc/udev.d directory. 

KERNEL=="spidev*", SUBSYSTEM=="spidev", GROUP="spi", MODE="0660"

add a spi groups and add user to this groups

## build

* make

## run

* ./eeprom (provides some infos and a usage list)

## background

Why to program a eeprom?

Well it started very harmless, with the promise to upgrade my video card.

My bad was that i did not lookup the exact chip version (of course not the just the type, that required to disassemble the whole card, and i hestitated on that, something that was required afterwards anyway...).

So it ended up with a bricked graphic card. And from there no suggestion did work.
I really like shared knowledge from the web, but in this case, it was a whole bunch of cooking instructions without naming the ingredients.
The following points may help someone with the same situtation:
* flashing a graphics cards is nice if you stick to to the same type and manufacturer
* make a backup of the content before flashing
* a wrong video-bios resulting in a pc not booting
* the nvflash tool is available in many shapes and colors (beware using -4...) 
* building a bootable stick with windows works best with rufus, download freeDos (minimal is sufficent) and with rufus select the .img
* the existence of some nice explanations how to unbrick suggests that this is not something two or three people may have expirienced
* the instructions with shortening some pins on the eeprom, did not make sense when trying to align that with the chip manual, at least not for the type of chip that was used on my card (one option might be to get your pc to boot and then remove the bridge, to allow programming (untested)...)
* using a raspi allowed to bring it back to life :) 

