Snake Game

A dynamic and engaging Snake Game developed in C++ using the SFML graphics library. The game offers three progressively challenging levels with unique mechanics like red and blue apples, bombs, and shrinking walls in Level 3. Built with object-oriented programming (OOP) principles and STL containers, it features smooth gameplay, robust collision detection, and a responsive user interface with intuitive menus.

Overview

Snake Game is a modern take on the classic Snake game, where players control a snake to collect apples, avoid bombs, and navigate dynamic environments. The game includes a polished main menu, level selection, pause menu, and help screen, all rendered using SFML. Three distinct levels escalate in difficulty with faster movement, bomb obstacles, and a shrinking play area, providing an exciting challenge for players.

Features

Three Challenging Levels:

Level 1: Standard gameplay with red apples (+1 point) and occasional big blue apples (+2 points, double growth).

Level 2: Increased speed, red apples (+2 points), blue apples (+4 points), and bombs that end the game.

Level 3: Fastest speed, shrinking walls every 5 seconds, red apples (+3 points), blue apples (+6 points), and bombs.

Interactive Menus: Main menu, level selection, pause menu, and help screen with mouse and keyboard support.

Dynamic Gameplay: Collect apples to grow and score, avoid bombs and collisions with walls or the snake’s body.

Responsive UI: Custom fonts, level-specific color schemes, and animated elements for a visually appealing experience.

Scoring System: Tracks score and apple count, with level-based point variations.

STL Integration: Uses STL vectors for snake segments and random number generators for object placement.

Controls

Arrow Keys: Move the snake (Up, Down, Left, Right).

P: Pause the game.

R: Restart the game (when in Game Over state).

ESC: Return to the main menu.

Enter: Select menu options.

Mouse: Click to navigate menus.

Dependencies

C++ Compiler: Compatible with C++11 or later (e.g., g++, MSVC).

SFML: Version 2.5 or higher (graphics, window, and system modules).

Fonts: Requires impact.ttf, arial.ttf, or other supported fonts (e.g., LiberationSans, DejaVuSans).


