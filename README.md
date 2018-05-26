# mqtt-rfid-corridor

**Sci fi corridor with MQTT/RFID activated doors**

I made this little demo for a presentation on IoT/MQTT, to demonstrate a lightsaber based authentication system.

The webpage is built using the [A-Frame](https://aframe.io/) framework.

**Demo**

Demo is here:

https://madmarcel.github.io/mqtt-rfid-corridor/

**Controls**

Arrow-keys - move

Enter - open the doors

Space - close the doors

R - trigger a bad read

Ctrl+Alt+Shift+i - open the A-Frame inspector where you can inspect and edit the scene.

You could view the scene using a Google Cardboard or Daydream, but it wouldn't be optimal, and you can't move around.
(I added the Daydream controller, but it shows up unpredictably)

**What it does**

The webpage listens for an MQTT signal to open or close the doors.

The signal will be sent by an arduino powered RFID reader, which is sitting behind a 3D printed Star Wars droid access panel.

For extra style points, the RFID tags are hidden inside the hilt of a handful of different BladeBuilder lightsabers.

(I used two tags per lightsaber to ensure I got a good read)

**Assets**

I made the corridor, doors and bulkhead using [TinkerCAD](https://www.tinkercad.com/) and then modified and scaled them using [Blender](https://www.blender.org).

Star Wars Droid Access Panel:

https://www.thingiverse.com/thing:1777318

**Other models**

Sci Fi Hangar

https://www.cgtrader.com/free-3d-models/interior/other/the-sci-fi-hangar-scene

Kameri Explorer

https://free3d.com/3d-model/kameri-explorer-57154.html
(I removed the words and logo from some of the textures)

Falcon T45 Rescue ship

https://free3d.com/3d-model/falcon-t45-rescue-ship-23161.html
(I removed the words and logo from some of the textures)

SFX:

Cribbed from a very old LucasArts game.

**Arduino code**

Arduino code is in the arduino folder.

I drilled out the red 'dots' on the access panel and glued two red push buttons in there.

Pressing either will trigger a door close event.

