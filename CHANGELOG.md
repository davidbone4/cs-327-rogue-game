2/7/2021
    => Create the git repository for the progect.
    => Began working on DungeonGenerator.c

2/8/2021
    => Rendered a dummy dungeon in the console.
    => Realized that I cant just use a 2d array of chars and I have to use an array of objects/cells. Looks like that will be a tomorrow progect.
2/9/2021
    => Restructured the dungeon as a 2d array of cell structs
    => used an enum type to show cell type.
    => implimented random room array generation (and used a struct for each room)
    => imputted all rooms into the dungeon structure
    => created logic for checking if any of the rooms were out of bounds, touching each other, overlapping, etc.
    => fixed issue with rand() generating the same numbers.
    => discovered that when I use srand() with the current time, it generates the same dungeon within each second.
2/9/2021
    => began work on corridor generation
    => changed the room generation size, number etc.
    => used mathematical distance to pave paths from room 0 -> 1 -> ... -> maxrooms -> 0
    => fixed issue that caused sections of dungeon to be cut off
    => implimented staircase gen
