1. How to build the project

- ★ Move to the PlugIns/include folder and decompress "boost_1_59_0.zip" file to the "boost_1_59_0" folder. The path should be like "PlugIns/include/boost_1_59_0/boost/...".

- Please set "DiceDungeonClient" project as a starter project and build with x64 configuration / Debug mode. Or you can execute with "DiceDungeonClient.exe" file in x64\Debug folder.
* Release mode is not working because of the library file of Thrift (libthrift.lib).

- If you want to access to the Leaderboard server, execute "x64\Debug\LeaderboardServer.exe" and Press "Connect" button in the right side of UI.

- GitHub Link:
https://github.com/kanious/DiceDungeon (final tag)
- UML Diagram file Link:
https://drive.google.com/file/d/1VYTI2U0iOVibw4_XtyeabavE4UNiMQ1W/view?usp=share_link

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

* I worked by myself and submitted the last Physics project separately in its dropbox.

< Topics >
1) Animation
   - Bone Animation
   - Skin Mesh Animation
   - Blended Animation
   Code location : Mesh.cpp in PumpkinEngine project (Mesh Loading)
                   Animation.cpp in PumpkinEngine project (Animation data Loading)
                   Animator.cpp in DiceDungeonClient project (Using animation)
                   BoneVertex.fx shader in x64/Assets/Shader folder

2) A.I
   - State Machine
   - Navigation Mesh (Tile) : Path-Finding / Path-Following
   Code location : StateMachine.cpp in DiceDungeonClient project
                   Tile.cpp / TileMaster.cpp in PumpkinEngine project
   When you press "Enemy Turn Start" button in the left side of UI, the enemy turn will be started.

3) Physics (Not in this project)
   - Vehicle
   - Character Controller






