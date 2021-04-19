2/7/2021
    => Create the git repository for the project.
    => Began working on DungeonGenerator.c

2/8/2021
    => Rendered a dummy dungeon in the console.
    => Realized that I cant just use a 2d array of chars and I have to use an array of objects/cells. Looks like that will be a tomorrow project.
2/9/2021
    => Restructured the dungeon as a 2d array of cell structs
    => used an enum type to show cell type.
    => implemented random room array generation (and used a struct for each room)
    => inputted all rooms into the dungeon structure
    => created logic for checking if any of the rooms were out of bounds, touching each other, overlapping, etc.
    => fixed issue with rand() generating the same numbers.
    => discovered that when I use srand() with the current time, it generates the same dungeon within each second.
2/9/2021
    => began work on corridor generation
    => changed the room generation size, number etc.
    => used mathematical distance to pave paths from room 0 -> 1 -> ... -> maxrooms -> 0
    => fixed issue that caused sections of dungeon to be cut off
    => implemented staircase gen
    => tested on pyrite server and got errors pertaining to the math.h library not being recognized. (this is the same problem mentioned in Piazza @94). I updated my make file to include -lm and it worked perfectly on the pyrite server.
    => update README.md and CHANGELOG.md
    => created zip and turned in assignment1.01
2/16/2021
    => added define statements to easily change dungeon parameters.
    => created load.c which currently take a file at $HOME/.rlg327/dungeon and reads The first 3 parameters
2/17/2021
    => added corridor, room and player character load from file.
    => added full load from file
    => combined load.c and dungeongenerator.c into one file.
    => changed dungeon to be a struct that included room information
    => added write method to write to a file
    => made dungeon a global variable
    => added command line argument support
2/18/2021
    => tested on Ubuntu
    => update README.md and CHANGELOG.md
    => created zip and turned in assignment1.02
2/22/2021
    => copied professor Sheaffer's algorithm to base my pathfinder logic.
    => created new file pathfinding.c
2/23/2021
    => created .h file to store all definitions for all files
    => changed hardness and other vars from signed to unsigned
    => correctly added non-tunneling path generation
    => correctly added tunneling path generation
    => tested on Ubuntu
2/24/2021
    => update README.md and turned in assignment1.03
3/2/2021
    => Created a monster struct and used an array within dungeon to store them
    => created monster.c and init_monsters method.
    => wrote logic to randomly determine type of monster and print hex numbers
    => updated path finding methods to take in a starting position
    => began fixing monsters and PC being stored in the grid.
3/2/2021
    => Had an issue with memory allocation with my path finding. I scrapped mine and used Sheaffer's
    => Wrote logic to move non tunneling monsters.
3/3/2021
    => Wrote logic to move tunneling monsters.
    => Began work on the Heap that runs the game
    => Changed the PC character to be a monster type
    => fixed issue where nontunneling monsters could not kill pc
    => added game over text.
    => Updated CHANGELOG.md and README.md
    => Tested on Ubuntu
    => Turned in Assignment1.04
3/7/2021
    => Removed border around dungeon display (finally)
3/8/2021
    => added ncurses into my project
    => rewrote my printdungeon to output with ncurses
    => added logic to move the player with the given keys
3/9/2021
    => added support to go up and down staircases and to generate a new dungeon each time
    => added rest and quit features
    => added monster list
    => created paginator for the monster list and added keys to control it
3/22/2021
    => ported to C++
    => changed make file
    => changed structs to classes and separated npc and pc classes
    => implemented fog of war
    => implemented new fog print method and fog toggle
    => created teleportation method
    => added random toggle in teleportation method
    => Tested on Ubuntu
    => Turned in Assignment 1.06
3/30/2021
    => Temporarily disabled game main method
    => added file reading
    => added dictionary to test which keyword was being used
    => implemented logic to skip an incomplete or a poorly formatted monster
    => printed to terminal
    => stored all monsters in a vector
    => Tested on Ubuntu
    => Turned in Assignment 1.07
4/5/2021
    => added Professor Sheaffer's parser into mine
    => added dice and definitions into my project
    => updated my npc class to include the new variables included
    => created an object class to store objects
    => added an object array in the dungeon class
    => reworked init_monsters method to create an array of monsters using the monsters description array vector
    => implemented logic to connect my path finding ai to the new monster architecture
4/6/2021
    => Added object initiation to the dungeon
    => fixed print method to print both new monsters and object
    => added color rendering
    => fixed npc typing to include the 5 new attributes
    => fixed unique monsters and artifacts
    => tested on Ubuntu
    => turned in Assignment 1.08
4/11/2021
    => added inventory and carry space to pc
    => added is picked_up boolean to object
    => implemented auto pick up for objects on the map
    => finished the 'w' equip command.
4/12/2021
    => finished the 't' unequip command.
    => finished the 'd' drop command
    => finished the 'x' delete command
    => finished the 'i' list inventory command
    => finished the 'e' carrying command
    => finished the 'I' inspect command
    => finished the 'L' Look command
    => added the speed of inventory
    => disabled NPC vs NPC
    => added hp logic for equipment
    => allowed enemies to hit you
    => added hp bars of the Player and the last hit npc at the bottom of the screen
    => added the win condition for killing the boss
4/18/2021
    => added money to the PC class
    => added the display of money to the front screen
    => implemented a sell feature by pressing 's'
    => made ACSII art to create the town
    => implemented a town method to allow the player to visit it by pressing 'c' and move around the street
    => fixed town render issue
    => created a shop that dynamically populates every time a player enters
    => allowed player to buy from the shop
    => created thielen where players can pay to heal
    => fixed issue where you could buy a heal when at max health
    => tested on Ubuntu
    => turned in Assignment 1.10
