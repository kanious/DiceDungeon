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

1) UML Diagram : There is a link above. Please use "open with diagrams.net". It was created by dividing into two tabs, Engine / Client. Some classes are not designed yet because I am not decided how to implement them. (in "Classes not yet structured" section of engine tab, physics system/network system/etc...)

2) About Git : I have been using this git since the beginning of the first semester when I've started to make my own engine. Please ignore last year's commit list.

3) GamePlay : You can move the characters in front of the screen by clicking on them. They all have a simple jump animation while moving.
And I am using Json and XML both for now. I plan to replace all files with Json or csv for use.

4) Design Patterns : I've used "Singleton", "Factory Method", "Component" design patterns.
* Singleton - Most of manager class in Engine/client are this pattern.
* Factory Method - DiceDungeonClient proejct has an Object Factory class. It is used when creating objects and I plan to continue extending it as more object classes are added.
* Component - The classes in the Component filter of the "PumpkinEngine" project are structured in this pattern.

5) Naming Rule
- Class of engine project : "C" in front of the class name
- Class of client project : No "C" in front of the class name
- Member Variable : "m_" is added in front of the variable name + Hungarian Notation + Camel Case
- Function : Pascal Case


