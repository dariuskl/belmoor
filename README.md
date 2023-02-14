Belmoor PM10A
===================

This is a reverse-engineered schematic of, and a custom firmware for the Belmoor
PM 10A, which is a DIN-rail mounted power metering mains load switch of obscure
origin.

A number of these devices have been purchased used. The original manufacturer
remains unknown besides its name "Belmoor Ltd.", which yields to plausible
results when researched.

The parts used in these devices are high quality (where it matters). The
construction quality varies unit-by-unit with some very "interesting" finds,
occasionally, such as electrolytic capacitors with zig-zagged leads standing
half a centimeter off the board. The routing is weird and often traces will
change their widths without apparent reason. The version number on the PCB
indicates at least five revisions, but still serious design issues remain, such
as the choice of optocoupler.


Application description and required hardware modifications
-----------------------------------------------------------

The application in the custom firmware is essentially a glorified soft-start
and voltage/current monitor with extended measurement and logging capabilities
via USB. For this purpose, it does however, require a few hardware
modifications. Most importantly, the relay must be modified to disconnect power
to the load, if the coil is not energized.


Considerations for an improved hardware design
----------------------------------------------

- bistable relay
- position of the isolation barrier


Device changelog
----------------

### S/N 1074

- Stuffed J6 with FTSH-105-01-F-DV-K-P and SWCLK, SWDIO series resistors as 10R.
- Stuffed SW1 with a simple generic push button.
- U4 (USBLC6-2) had a short from VBUS to GND. Removed.

### S/N 1076

- Stuffed J6 with FTSH-105-01-F-DV-K-P and SWCLK, SWDIO series resistors as 10R.
- Resoldered mains-side power supply capacitors, which were stood off the board.
- U4 prevented device from enumerating. Removed. Exact fault of the IC still
  to be determined.

### S/N ????

- Desoldered relay, trimmed off NC pins, soldered back and bridged NO pins to
  NC pads, thus inverting relay function in order to make sure it is turned off
  on start.
