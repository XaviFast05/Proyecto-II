# Mega Man Mini
## Project
https://github.com/Belin1234/2D-Platformer.git



## Team members
Adrià Belinchon Pérez

Claudia Ruiz Muñoz

### Github acount

Belin1234

https://github.com/Belin1234


cla4d1a

https://github.com/cla4d1a

## Description

This game is inspired by Mega Man video games; it’s a platformer where you must dodge and jump over various obstacles to reach the end of the level.

## Key Features

 - Move Right, left, to advance.
 - Jump across the different platforms.


## Controls

 - Key left-> move left
 - Key right  -> move right
 - Key up -> move up in god mode
 - Key down -> move down god mode
 - H -> Show/ hides the help menu with debug controls.
 - E -> Punch.
 - F -> Shoot.
 - Esc -> Pause.
 - F1 -> Beginning of the first level.
 - F3 -> Beginning of the current level.
 - F5 -> Save.
 - F6 -> Load.
 - F7 -> Next checkpoint.
 - F8 -> Gui state.
 - F9 -> Displays the colliders/ game logic.
 - F10 -> Activates " God Mode".
 - F11 -> Enables/ disables FPS cap at 30.
   
 
## Features implemented
 - Follow Camera: The camera follows the main character troughout the map.
 - TMX-based Map (Tiled): The map is loaded from a .tmx file, which includes layers for terrain and colliders.
 - Dynamic Colliders; Platform colliders are created dynamically upon loading the map using Box2D.
 - Player Animations: The player has animations for walking,jumping,dying and attacking.
 - Controls : WASD keys for movement and space key to jump.
 - External Configuration: initial position, player speed, amimations are loaded from external files.
 - FPS Optimization :The game maintains a stable cap of 60 FPS without vsync, and the window displays current FPS, average 
                     FPS, and ms per frame
 - Normalized Movement: All game movement is normalized using deltaTime, ensuring the same movement speed across different CPU speeds.
 - Enemics: There are flying enemies and walking enemis.
 - Pathfinding: The enemies have a idle state and when it detects the player, the enemy starts moving towards to the player position.
 - Load and save: It saves the player’s position.
 - Music and sounds effects: Added music to level, sound effects of attacking and death.
 - Final level: When the player arrives at the end sound is heard and closes automatically
 - Add Gui screen and make them functional:
    - Intro screen.
    - Title screen
    - Gameplay HUD
    - Screen pause menu
    - Die screen
    - End screen/ Level completed
 - Shots: Attack to shoot enemies.
 - Game items: Grab different objects to refill your shooting ammunition, increase your life or 
   get points.
