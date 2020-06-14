# Mini Rogue Game 
[Demonstration]()

Assigned as project 3 for CS 32, the mini rogue CLI game was designed as a simplified version of the Rogue video game from the 1980s. In this game, users can traverse randomly generated dungeons, fighting monsters, picking up weapons and scrolls, and descending levels until they either perish or pick up the golden idol, thus emerging victorious. 
#### Key Features 
 * **Golbin movement:** one of the monsters in this game, the goblin, moves via a function that implements BFS in order to find the shortest path to the player. Goblins can "smell" the player if he/she is within 15 steps and will always move along a path that brings them closer to the player. 
 * **Dungeon Generation***: in order to randomly generate dungeons and corridors between them, I implemented a recursive function that attempts to generate as many rooms as possible within the dungeon grid and then draws corridors to connect them. Lots and lots of bounds checking!
#### Concepts Used
* OOP (inheritace, polymorphism)
* Recursion 
* STL containers
* BFS 