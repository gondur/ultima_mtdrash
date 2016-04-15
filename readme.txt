Ultima: Escape from Mount Drash:
---------------------------------

"Ultima: Escape from Mount Drash" is a PC port of the old VIC20 game that
will allow people to run the game without emulation on a modern PC.

Ultima: Escape from Mount Drash is licensed under the GNU General Public License.
Please read the file Copying for more information.

Ultima: Escape from Mount Drash is Copyright (C) 2003 Kasper Fauerby

kasper@peroxide.dk
www.peroxide.dk



Mount Drash - the story:
-------------------------

Long believed to be "Vaporware" Ultima: Escape from Mount Drash is a simple
maze game that shares nothing more than the name with the famous Ultima series.
The original author, Keith Zabalaoui, apparently knew Richard Garriott - the
father of the Ultima name - as a friend, and Sierra, the publisher of "Mount Drash",
could easily see the benefit of putting the Ultima label on one of their products.

Due to it's rarity the game has since been labeled as the "holy grail of classic
game collecting" and an original copy of the game could sell for an insane amount. See:

http://home.hiwaay.net/~rgregg/ultima/collectibles/Title_Other.html 

for more details on this story.

My motivation for doing a PC port of the game was partly to let people who are curious 
about what the game looks like give it a spin and partly just for the fun of it.




How to use:
------------

I've included the source code to the game with this package and it should compile 
out of the box with Mingw on the windows platform. There are no platform specific 
code used though so with minor tweaks in the Makefile it should easily compile on 
all platforms with a C++ compiler and a SDL version (for example, Linux).

Also included is a precompiled binary for the Windows platform.

See the included jpg picture 'card-inside.jpg' of the original manual sheet to see 
how the game is played. I've added a few keys to allow for easier navigation in 
the maze though so you can also just the forward and backward keys for movement 
in this version.

Q or ESCAPE quits the game.

Fighting is slightly easier in this version than on the VIC20 - not because the AI 
or combat algorithm has been changed but because where the PC listens for input each 
frame (to change fighting stance) the VIC20 would only check for input once every 
second or so. You should not miss that "feature" though :



How I did it:
--------------

So how did I manage to port the game anyway? (not important at all, but those who know
me knows that I likes to babble ;)) 
Well, it all started as I came across an image of the original game which I could run 
in a VIC20 emulator. After I decided to try and port it I looked around on the net for 
tools that would help me and by chance I mentioned my idea to Michael Fink whom I know
from the "Underworld Adventure" project (at http://uwadv.sourceforge.net - check it out, 
it's really cool!) and he told me he could easily extract the individual files from the 
image. That done we discovered that the game consisted of three files: 2 written in BASIC 
and a third which we suspected to contain part data and part machine code used for the 
time-critical inner loops. I annotated the two basic files and got an idea of the overall 
flow of the game code. 

I also disassembled the binary file and spent a few hours annotating and analyzing the 
machine code, using a VIC20 reference manual to look up register addresses and a reference 
of the 6502 processor and its machine code language to decode the code. The binary file 
turned out to have only two entry points, one to a function that would clear part of the 
VIC20 video memory (BASIC must have been too slow for this task) and one to a function 
which installs a custom interrupt handler to control sound, the timer (the time value that's 
counting down in the game) and to rotate the color of the character used as the exit gate 
on each level. The rest of the binary file consisted of 7 tracks of music.

None of the code in the binary file was terribly important for a port though and as such none
of it has been used directly in the PC version. A modern day PC doesn't need machine code to 
clear 198 bytes of memory anymore ;) I decided to cut away the music from the PC version because 
I don't know how to properly emulate the sound the VIC20 which has 3 channels for playing 
different notes and one for making noise.

The only "data" that I had to use directly from the VIC20 version was the strings used 
in the BASIC programs to draw the graphics (the monsters, title screen and 3d view) 
because the VIC version of ASCII strings can contain control codes to, well, control 
how the string is printed. It can set stuff like the color being used and even move 
the cursor around the screen. The easiest way to get the graphics right was to use a 
string parser that would take a PET ascii string and process it with all its control 
codes etc. The characters are then drawn to a virtual VIC20 screen which finally, at 
the end of each frame, blitted to a SDL surface which can be shown on the PC graphics 
card. 

The game itself has been partly ported directly from the BASIC source and partly 
redesigned and reimplemented by me. I didn't change any of the algorithms used though 
to keep it as close to the original as possible.
