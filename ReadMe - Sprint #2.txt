1. How to build the project

- Please set "DiceDungeonClient" project as a starter project and build with x64 configuration / Debug or Release mode. Or you can execute with "DiceDungeonClient.exe" file in x64\Debug(or Release) folder.

- GitHub Link:
https://github.com/kanious/DiceDungeon

- UML Diagram file Link:
https://drive.google.com/file/d/1VYTI2U0iOVibw4_XtyeabavE4UNiMQ1W/view?usp=share_link

- There are 3 projects.
  DiceDungeonClient : Main game client project.
  MapEditor : Only for map data editing and it is not finished yet.
  PumpkinEngine : Engine project(DLL).



2. User Input Option

(Added)
* R : Roll 2 dice


(Same with Sprint #1)
* Mouse Click :
  Select Target or Move Target
  If you click a character, the character will set as a target.
  If you click the ground and have a target already, the target will move to the position.
  (LB) - Move to clicked position
  (RB) - Move to the center position of the clicked tile (like grid)

* 1 : Release the target
* ESC : Close the program

< For Camera >
* WASD : Moving Camera (look/right direction).
* Space/Left Shift : Up/Down Camera (up direction).
* Alt : Enable/disable mouse. Rotating camera with mouse move when mouse is disabled.



3. Description

1) Content Pipeline
Compressed and converted textures are saved in each folder in x64\Assets\Texture folder.

- Lossy compression files : x64\Assets\Texture\Lossy
- Lossless compression files : x64\Assets\Texture\Lossless
- Android(.ktx) : x64\Assets\Texture\Convert\Android
- iOS(.pvr) : x64\Assets\Texture\Convert\iOS
- Windows(.dds) : x64\Assets\Texture\Convert\Windows

I only uploaded Lossy folder and original folder because full files are too many and big.
Therefore, it may take a long time to convert when initially compiled.

Current projects are using Lossy version textures to reduce the loading time.



2) Physics
Added physics to roll dice in "DiceDungeonClient" project.
All core code is in the pumpkin engine project. (in the 05.IndependantFunctions\Physics filter)

If you press 'R', 2 dice will be rolled toward direction the camera is looking.
If you press the Buttons (Roll2, Roll3, Roll5) in the UI, the dice for each number are rolled.



3) Core Game Mechanic
During this period, I focused on making the map editor.

Update List
- Add map editor functions
   - Game object control by layer
   - Change layer
   - Object snap function (correction according to the angle in the case of walls)
   - Object information copy function
   - Object alpha selection function
   - If click right mouse button, the selected target returns to its original position
- Add frame buffer object
- Add dice system with physics
- Mesh loading with Assimp
- Making the demo map and tiles (In progress)






