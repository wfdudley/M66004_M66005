# M66004_M66005
Arduino driver for M66004/5 vacuum fluorescent display driver. 
Only tested with ESP32.  You may have to fiddle with the SPI code
for other processors.

This started out with code written by george9801 (sorry, no other information
available on george).  He had the right idea, but I gave up trying to
understand his initialization sequences and just re-wrote it all.  I
converted to C++ object, and added individual functions to set brightness,
define custom characters, and clear the display.

Because the VFD in my (dead) Onkyo receiver uses the 16th character
position to drive the (34!) icons on the display, my code allows
one to reserve the 16th position for that use.  (Set ICONS_OFFSET
to 1 to enable this.)

Also, my display was wired by (I assume) a native Japanese speaker to read
from right to left, so my code allows you to change the direction from "normal"
left to right by defining RT_TO_LEFT_MODE.

Because I found the datasheets for these parts hard to find, I have saved copies 
<A HREF="https://www.dudley.nu/data-sheets/">here</A>.

Bill Dudley
January 9, 2025
