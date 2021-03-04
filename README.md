# cs-327-rogue-game by David bone

## Assignment 1.04

This assignment added monsters and allowed those monsters to move. This utilized a proirity queue using the heap.c file.
I also had a memory issue when using my pathfinding algorithm from last week, so I decided to scrap it and use Professor Sheaffer's logic.

I separated the monsters into a tunneling and non-tunneling method. If a monster was intelligent, it always used the pathfinding algorithm. If a monster was not intelligent, then it used a simple Pythagorean Therom method to find its way to the PC

If a monster was Telepathic, it always used the current location of the PC to pathfind. If it was not, I used a memory variable as well as a islineofsight method to determine if or where to move.

I decided to not implement logic to have the character move.

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

If --nummon is not used, the default value is 10.

Tested on Ubuntu

Thank you!

## Assignment 1.03

This assignment adds distance map generation for both tunneling and non-tunneling. Both are automatically output when the program is ran.

All argument logic still applies.

USAGE:

    >make
    > ./rlg327 <arguments>

NOTE: tested on Ubuntu

Thank you!

## Assignment 1.02

This assignment adds file read and write capabilities to the file DungeonGenerator.c.

It always reads or writes a file from the path

    $HOME/.rlg327/dungeon

I have included the following commandline arguments:

    "--load" => loads the dungeon at said path and displays it
    "--save" => saves dungeon to said path before exiting

If now "--load" argument is set, it will generate a new dungeon using the algorithm I created last week. You are able to use both arguments in one line in any order. This will rewrite the loaded file every time it is ran.

USAGE:

    >make
    > ./rlg327 <arguments>

NOTE: I tested this program on Ubuntu and it was fully functional. On the pyrite server, I {Segmentation fault(core dumped)} error. I assume this is something to do with creating a .rlg327 folder in home. It is fully functional in Ubuntu.

ALSO NOTE: The terminal window must be AT LEAST 82 x 24 as I have a border around the dungeon output. (maybe I should delete the border...)


Thank you!





## Assignment 1.01

This is the First assignment in the Rogue Game project! Booyah!

So far I have only created one file: DungeonGenerator.c. This file randomly generates a dungeon according to the guidelines listed in the assignment and prints it to the console. It was tested on the pyrite server(see CHANGELOG.md).

Some things to note:
   1. The program first randomly generates 6-10 rooms of varying size and places them inside the dungeon such that no rooms directly touch and there is a band of 1 cell thick around the perimeter of the dungeon data structure

   2. I then use a mathematical distance algorithm to connect all rooms with corridors (I can see that it is ugly, I will improve it later).

   3. I finally randomly place an UP staircase and a DOWN staircase in any of the rooms. I understand that it is not allowed to go into a corridor, however in professor Sheaffer's example, he has a staircase in corridors

I have implemented a simple hardness quality in all of the cells, but have not created a dynamically allocated room array (or perhaps I have, I am not exactly sure)

USAGE:

    >make
    >./generator.out

NOTE: The terminal window must be AT LEAST 82 x 24 as I have a border around the dungeon output.

Enjoy!
