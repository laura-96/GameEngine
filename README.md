# GameEngine

GameEngine is a 3D Game Engine developed as a project for Video Games Engine subject in UPC's centre, CITM.

It currently functions as a scene viewer. 3d models are loaded in a scene where it is possible to navigate throug keyboard (asdw). These models must be saved on Assets/ directory, and its textures, to the Textures/ one.
The editor shows game objects' hierarchy, which can be modified through a panel that will appear by selecting them on the hierarchy window.

If in File menu Save is selected, the current scene will be saved. To load any stored scene, Load option must be selected. To create a game object in game, click on right button anywhere in the screen and an option panel will appear.
In case any texture resource is modified during the game, this one will be reimported and displayed at the moment.

After pressing play, none of the transformations will affect the original object, which will get back its properties once stopped. Pause option, for now, pauses the objects update until play is pressed.

Multiple components can use one same resource, and this resource won't be cleared if any object is using it.

GameEngine doesn't allow multitexturing at the moment.

## Author:

Laura Ripoll Galan

## Github repository:

https://github.com/laura-96/GameEngine
