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
2/17/2022
    => added corridor, room and player character load from file.
    => added full load from file
    => combined load.c and dungeongenerator.c into one file.
    => changed dungeon to be a struct that included room information
    => added write method to write to a file
    => made dungeon a global variable
    => added command line argument support
