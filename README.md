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
