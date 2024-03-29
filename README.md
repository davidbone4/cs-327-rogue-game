# cs-327-rogue-game by David bone

## Assignment 1.10

Here is my final assignment for my Rouge-like game.

For this assignment, I decided to flesh out the 'value' aspect of objects by creating a buy and sell mechanic, complete with PC money and a town you can visit to spend said money on loot or potions to heal yourself.

To sell an item, from the main screen, press 's'. You are them prompted for a carry slot to sell from. This method works like the delete method, however it increments your money with the value of the sold object

To spend your money, from the main screen, press 'c'. This teleports you to a town with two places to visit, the Armor store and Thielen student health center. To enter a store, use the same movement keys from the game to move along the street. Then enter a door of one of the stores. Press Escape to leave town.

In the armor shop, you are presented with 3 randomly generated items that you can purchase for the displayed cost. Buying an item places it in the first available carry spot. Press escape to leave the store.

In Thielen, you can buy health items to immediately regenerate health. The minor health potion regenerates 50 health, the major health potion regenerates 100 health, and the coveted Clone Cone brings you to max health. Press escape to leave Thielen.

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

Tested on Ubuntu

PS: thank you for all your hard work as a TA for this class. It has been a very fun class and my favorite of the semester by far.

## Assignment 1.09

The penultimate assignment had me adding many object related commands to the dungeon. All now work as outlined in the assignment pdf. All must be used when looking at the dungeon and not having any other commands open. For instance, when the game is prompting the user for a carry slot after pressing the 'w' command, you cannot immediately look at the inventory, you must first press esc to cancel the wear command. All commands may be cancelled or exited using escape.

At the bottom of the screen, the player health will always be displayed, followed by the last damaged monster or the first monster in the dungeon array if no monster has been damaged. All damage, speed and health calculations work as outlined.

The game is won by killing a BOSS (spongebob squarepants), but that may be very difficult, as the BOSSes are rare. Once this happens, the game displays the 'crown' win graphic and then exits.

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

Tested on Ubuntu

## Assignment 1.08

This assignment had me using the new file parsing to add different types of monsters and objects to the dungeon.

I began by using Professor Sheaffer's parser for both objects and monsters. This took some time to read through and understand how to fit with my current architecture.

The dungeon works as it did in Assignment 1.06, but now it renders the new monsters and objects into the dungeon space.

Both the monsters and objects include the symbol, color, and abilities specified when generated. All other values are stored in their objects within the dungeon architecture. This includes rolling the values that are meant to be rolled at generation.

I also fixed an error I had in 1.06 where I had mapped the fog of war toggle to 't' instead of 'f'. Everything should work as expected.

I opted not to include any optional functionality in this assignment. Thank you!

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

Tested on Ubuntu

## Assignment 1.07

This assignment consisted of parsing a text document for information on various monsters. I did not implement the optional Item definitions.

When ran, the program searches for a file called "monster_desc.txt" in the home folder of this assignment, stores the data of all complete monsters in a vector, prints each to the terminal and exits.

The order in which the program prints to terminal is inline with that of the table in the assignment PDF. (I noticed that this order was different that the order the example output was in, notably, the symbol parameter was out of place. I formatted my output against the table.)

If a monster is missing a parameter or has more than one definition of a parameter, it is thrown out and the next monster is read.

USAGE:

    >make
    > ./rlg327

Tested on Ubuntu

## Assignment 1.06

There is now a default fog of war view in the game. This view is toggled by 't' during the players turn. You do not waste a turn to do this.

There is also now now a teleportation key 'g' that activates a selection window, with the new location denoted with '*'. You are allowed to teleport into rock, but not immutable rock. If a rock is immutable, you will not be able to select it.

Pressing 'r' while selecting a teleportation location will teleport the PC to a random non-immutable cell.

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

Tested on Ubuntu

## Assignment 1.05

I have now added user input into the dungeon game. I have added all Keys that were specified.
The terminal output is now using Ncurses rather than stdio. This took some reworking of my print dungeon method but in the end, it worked out really well

If a key is pressed that is not one specified, the turn is saved and the header prompts the user that the input was not recognized. This goes for the main screen, not the monster list screen.

I have used the header to tell the user what action has just taken place. The bottom two lines on the dungeon screen are place holders for future assignments.

NOTE: I have not updated the save or load features, per Professor Sheaffer's instruction

USAGE:

    >make
    > ./rlg327 --nummon <desired integer of monsters>

Tested on Ubuntu

Halfway there!! (with assignment 1 at least...)


## Assignment 1.04

This assignment added monsters and allowed those monsters to move. This utilized a priority queue using the heap.c file.
I also had a memory issue when using my pathfinding algorithm from last week, so I decided to scrap it and use Professor Sheaffer's logic.

I separated the monsters into a tunneling and non-tunneling method. If a monster was intelligent, it always used the pathfinding algorithm. If a monster was not intelligent, then it used a simple Pythagorean Theorem method to find its way to the PC

If a monster was Telepathic, it always used the current location of the PC to path-find. If it was not, I used a memory variable as well as a islineofsight method to determine if or where to move.

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
