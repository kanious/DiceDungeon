1. How to build the project

- ★ Move to the PlugIns/include folder and decompress "boost_1_59_0.zip" file to the "boost_1_59_0" folder. The path should be like "PlugIns/include/boost_1_59_0/boost/...".

- Please set "DiceDungeonClient" project as a starter project and build with x64 configuration / Debug mode. Or you can execute with "DiceDungeonClient.exe" file in x64\Debug folder.
* Release mode is not working because of the library file of Thrift (libthrift.lib).

- If you want to access to the Leaderboard server, execute "x64\Debug\LeaderboardServer.exe" and Press "Connect" button in the right side of UI.

- GitHub Link:
https://github.com/kanious/DiceDungeon

- There are 4 projects.
  DiceDungeonClient : Main game client project.
  MapEditor : Only for map data editing
  LeaderboardServer : Leaderboard server project.
  PumpkinEngine : Engine project(DLL).



2. User Input Option

* Mouse LB Click : Select tile
* Mouse RB Click : Deselect tile
* ESC : Close the program

< For Camera >
* WASD : Moving Camera.
* QE : Rotate Cemera by 90 degree.
* Mouse Wheel Drag : Zoom In/Out.
  


3. Description

You can see the debug deferred rendering textures if you check the checkbox in the middle of the left side of UI.

Noise screen option is near it as well.

< Added Functions >
Deferred rendering
Deferred lighting
Thread
Noise screen effect
GPU skinning and blending for skinned mesh
.
.
.





