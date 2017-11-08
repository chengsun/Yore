#Yore

Yore is an under-developement roguelike game.

##Build instructions

Yore should run on any machine which is supported by SDL. It has only been tested on those mentioned below, however.

####To build for Linux:

1.  you must have gcc and SDL installed (you probably will anyway);
2.  run
<pre>$ make</pre>

####To build for Windows:

1.  you must have MinGW and SDL installed, with SDL.dll in your bin/ folder;
2.  run
<pre>&gt; PATH &lt;your mingw install path&gt;\bin
&gt; build-win</pre>
If that doesn't work then try
<pre>&gt; mingw32-make</pre>

The binary "Yore" (or "Yore.exe") is in the bin/ directory.

###Yore runs (tested) on the following platforms:

 *  Arch Linux

Feel free to add to the list if you have a different operating system handy.

##Gameplay

Gameplay in Yore is largely similar to NetHack. 

###Movement

Movement with the hjkl set (yubn for diagonals) is supported.
The arrow keys move the view around (the map is probably too large to fit all on to the screen at once).

###Miscellaneous

Various other commands are (in no particular order):
<pre>,      pick up
:      look down
;      look around
i      view inventory
w      wield a weapon
d      drop an item from your inventory

F      force attack in a direction
m      force movement in a direction
&lt;Esc&gt;  view status screen
s      view skills menu
f      enter/exit focus mode
p      shield (protect) in a direction

S      save and quit
Q      quit permanently</pre>

##License

Yore is distributed under the GNU GPL v3, a copy of which may be found [here](http://www.gnu.org/licenses/gpl.html).

