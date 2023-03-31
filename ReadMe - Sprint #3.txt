1. How to build the project

- ★ Move to the PlugIns/include folder and decompress "boost_1_59_0.zip" file to the "boost_1_59_0" folder. The path should be like "PlugIns/include/boost_1_59_0/boost/...".

- Please set "DiceDungeonClient" project as a starter project and build with x64 configuration / Debug mode. Or you can execute with "DiceDungeonClient.exe" file in x64\Debug folder.
* Release mode is not working because of the library file of Thrift (libthrift.lib).
  Keep researching to fix this problem...

- Execute "x64\Debug\LeaderboardServer.exe" first before run the client project. If you ran the client project first, use "Connect" button in the right side of UI after executing server project.

- GitHub Link:
https://github.com/kanious/DiceDungeon (milestone_3 tag)
- UML Diagram file Link:
https://drive.google.com/file/d/1VYTI2U0iOVibw4_XtyeabavE4UNiMQ1W/view?usp=share_link

- There are 4 projects.
  DiceDungeonClient : Main game client project.
  MapEditor : Only for map data editing
  LeaderboardServer : Leaderboard server project.
  PumpkinEngine : Engine project(DLL).



2. User Input Option

(Added)
* Animation Play Keys (with sound effect)
  F1 : Walk
  F2 : Run
  F3 : Attack1
  F4 : Attack2
  F5 : Grab
  F6 : Pick up
  F7 : Hurt
  F8 : Death
* P : Start to play background music (loop)


(Same with Sprint #1)
* Mouse Click :
  Select Target or Move Target
  If you click a character, the character will set as a target.
  If you click the ground and have a target already, the target will move to the position.
  (LB) - Move to clicked position
  (RB) - Move to the center position of the clicked tile (like grid)

* ESC : Close the program

< For Camera >
* WASD : Moving Camera.
* QE : Rotate Cemera by 90 degree.
* Mouse Wheel Drag : Zoom In/Out.



3. Description

1) Persistence
All related code are in 
	PumpkinEngine > 05.IndependantFunctions > DAO filter
	DiceDungeonClient > 98.Manager > ScoreManager class
- DiceDungeonClient is using "score.db" file in x64/Assets folder.
- LeaderboardServer is using "ranking.db" file in x64/Assets folder.


2) Networking
All related code are in 
	LeaderboardServer Project
	DiceDungeonClient > 05.Leaderboard filter (All files compiled with Thrift)
	DiceDungeonClient > 98.Manager > LeaderboardManager class
- Thrift file is in x64/Assets folder.


3) Audio & Scripting
All related code are in 
	PumpkinEngine > 05.IndependantFunctions > LuaBrain Filter
I'm using this script the most in here:
	DiceDungeonClient > 03.Animator > Animator class
- Lua script file is in x64/Assets folder.
- I'm playing a sound effect like footstep to a specific keyframe in the player's animation (Pressing Key "F1 ~ F8").
- And play the background music also (Press key "P").






