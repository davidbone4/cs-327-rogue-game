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
