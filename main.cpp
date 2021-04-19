#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>

#include "dungeondefinitions.h"

int seed;
int fog;
heap_t h;
dungeon_type dungeon;
int nummon;

void teleportPC(dungeon_type *d);
int player_turn(dungeon_type *d);
void printFogDungeon(dungeon_type *d);
void io_init_terminal(void);
void end_game();
objects_inventory equip(dungeon_type *d);
objects_inventory getinventoryslot(dungeon_type *d, object o);
int unequip(dungeon_type *d);
int dropitem(dungeon_type *d);
int deleteitem(dungeon_type *d);
void listinventory(dungeon_type *d);
void listcarry(dungeon_type *d);
void inspect(dungeon_type *d);
int inspectMonster(dungeon_type *d);
int sellitem(dungeon_type *d);
void gotoTown(dungeon_type *d);
void armorShop(dungeon_type *d);
void visithealthcenter(dungeon_type *d);
void win_game();
int main(int argc, char const *argv[])
{

    io_init_terminal();
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);

    int boolsave = 0;
    int boolload = 0;
    nummon = 10;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--save") == 0)
        {
            boolsave = 1;
        }
        else if (strcmp(argv[i], "--load") == 0)
        {
            boolload = 1;
        }
        else if (strcmp(argv[i], "--nummon") == 0)
        {
            nummon = (int)strtol(argv[i + 1], NULL, 10);
            i++;
        }
    }

    seed = (unsigned)time(NULL);
    generate(seed);

    fog = 1;
    dungeon.PC.alive = 1;
    dungeon.PC.nextturn = 0;
    dungeon.PC.sequencenumber = 0;
    dungeon.PC.speed = 10;
    dungeon.PC.isNPC = 0;
    dungeon.PC.hp = 200;
    dungeon.PC.damage = dice(0, 1, 4);
    dungeon.PC.money = 0;

    for (int i = 0; i < 12; i++)
    {
        dungeon.PC.inventory[i] = object();
        if (i > 9)
        {
            continue;
        }
        dungeon.PC.carry[i] = object();
    }
    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            dungeon.PC.map[i][j].type = ROCK;
        }
    }

    dungeon_type *d;
    d = &dungeon;

    parse_descriptions(d);

    h = init_monsters(d, nummon);

    const char *header = "Welcome to David Bone's cs327 project!";
    for (int i = 0; i < strlen(header); i++)
    {
        mvaddch(0, i, header[i]);
    }
    run_game(d);
}

void run_game(dungeon_type *d)
{

    static npc *c;
    while ((c = (npc *)heap_remove_min(&h)))
    {

        if (c->isNPC == 0)
        {
            int out = 0;
            if (c->alive == 0)
            {
                end_game();
                break;
            }
            while (!out)
            {
                out = player_turn(d);
            }
        }
        else if (c->type[TUNNELING] == '1')
        {
            move_monster_tunneling(d, c);
        }
        else
        {
            movemonsternontunneling(d, c);
        }

        c->nextturn = c->nextturn + (1000 / c->speed);
        if (c->alive == 1)
        {
            c->hn = heap_insert(&h, c);
        }
    }
}

int player_turn(dungeon_type *d)
{

    int out = 1;
    int teleported = 0;

    for (int i = d->PC.pos.y - 2; i <= d->PC.pos.y + 2; i++)
    {
        for (int j = d->PC.pos.x - 2; j <= d->PC.pos.x + 2; j++)
        {

            if (i < 0 || j < 0 || i >= DUNGEON_Y || j >= DUNGEON_X)
            {
                continue;
            }

            if (islineofsight(d, i, j, d->PC.pos))
            {
                d->PC.map[i][j].type = d->map[i][j].type;
            }
        }
    }

    if (fog)
    {
        printFogDungeon(d);
    }
    else
    {
        printDungeon(d);
    }

    for (int i = 0; i < d->num_objects; i++)
    {
        if ((d->objects[i].pos.x == d->PC.pos.x) && (d->objects[i].pos.y == d->PC.pos.y) && d->objects[i].picked_up == false)
        {
            for (int j = 0; j < 10; j++)
            {
                if (d->PC.carry[j].type == objtype_no_type)
                {
                    d->objects[i].picked_up = true;
                    d->PC.carry[j] = d->objects[i];
                    break;
                }
            }
        }
    }

    d->PC.speed = 10;
    int maxhealth = 200;
    for (int i = 0; i < 12; i++)
    {
        d->PC.speed += d->PC.inventory[i].speed;
        maxhealth += d->PC.inventory[i].hit;
    }

    std::string hp = "Player Health: ";
    hp += std::to_string(d->PC.hp);
    hp.push_back('/');
    hp += std::to_string(maxhealth);

    for (int i = 0; i < hp.length(); i++)
    {
        mvaddch(22, i, hp.at(i));
    }

    std::string coins = "Player Money: ";
    coins += std::to_string(d->PC.money);
    coins += " coins";
    for (int i = 0; i < hp.length(); i++)
    {
        mvaddch(22, i, hp.at(i));
    }

    for (int i = 80 - coins.length(); i < 80; i++)
    {
        mvaddch(22, i, coins.at(i - (80 - coins.length())));
    }
    // for (int i = 0; i < strlen(placeholder); i++)
    // {
    //     mvaddch(23, i, placeholder[i]);
    // }

    refresh();

    int ch = getch();
    char header[100];

    uint8_t yNext = d->PC.pos.y;
    uint8_t xNext = d->PC.pos.x;

    switch (ch)
    {
    case '7':
    case 'y':
        sprintf(header, "Moved PC one cell to the Upper Left.");
        yNext--;
        xNext--;
        break;
    case '8':
    case 'k':
        sprintf(header, "Moved PC one cell Up.");
        yNext--;
        break;
    case '9':
    case 'u':
        sprintf(header, "Moved PC one cell to the Upper Right.");
        yNext--;
        xNext++;
        break;
    case '6':
    case 'l':
        sprintf(header, "Moved PC one cell to the Right.");
        xNext++;
        break;
    case '3':
    case 'n':
        sprintf(header, "Moved PC one cell to the Lower Right.");
        yNext++;
        xNext++;
        break;
    case '2':
    case 'j':
        sprintf(header, "Moved PC one cell Down.");
        yNext++;
        break;
    case '1':
    case 'b':
        sprintf(header, "Moved PC one cell to the Lower Left.");
        yNext++;
        xNext--;
        break;
    case '4':
    case 'h':
        sprintf(header, "Moved PC one cell to the Left.");
        xNext--;
        break;
    case '>':
        if (d->map[d->PC.pos.y][d->PC.pos.x].type == DOWN)
        {
            heap_delete(&h);
            seed = (unsigned)time(NULL);
            dungeon = generate(seed);
            for (int i = 0; i < DUNGEON_Y; i++)
            {
                for (int j = 0; j < DUNGEON_X; j++)
                {
                    dungeon.PC.map[i][j].type = ROCK;
                }
            }
            h = init_monsters(&dungeon, nummon);
            clear();
            sprintf(header, "Moved down the stairs.");
            for (int i = 0; i < strlen(header); i++)
            {
                mvaddch(0, i, header[i]);
            }
            run_game(&dungeon);
            return 1;
        }
        else
        {
            sprintf(header, "Must be on downward stairs.");
        }
        break;
    case '<':
        if (d->map[d->PC.pos.y][d->PC.pos.x].type == UP)
        {
            heap_delete(&h);
            seed = (unsigned)time(NULL);
            dungeon = generate(seed);

            for (int i = 0; i < DUNGEON_Y; i++)
            {
                for (int j = 0; j < DUNGEON_X; j++)
                {
                    dungeon.PC.map[i][j].type = ROCK;
                }
            }
            h = init_monsters(&dungeon, nummon);
            clear();
            sprintf(header, "Moved up the Stairs.");
            for (int i = 0; i < strlen(header); i++)
            {
                mvaddch(0, i, header[i]);
            }
            run_game(&dungeon);
            return 1;
        }
        else
        {
            sprintf(header, "Must be on upward stairs.");
        }
        break;
    case '5':
    case ' ':
    case '.':
        sprintf(header, "Waited a turn.");
        break;

    case 'm':
    {
        int numPages = nummon / 23 + 1;
        int curPage = 1;
        int done = 0;
        while (!done)
        {
            char monster_string[100];
            sprintf(monster_string, "List of Monsters");
            clear();
            for (int i = 0; i < strlen(monster_string); i++)
            {
                mvaddch(0, i, monster_string[i]);
            }
            for (int i = (curPage - 1) * 22; i < nummon && i < 22 * curPage; i++)
            {
                if (d->monsters[i].alive == 1)
                {
                    int Ydistance = d->PC.pos.y - d->monsters[i].pos.y;
                    int Xdistance = d->PC.pos.x - d->monsters[i].pos.x;
                    char NS[6];
                    char WE[5];
                    if (Ydistance < 0)
                    {
                        sprintf(NS, "south");
                        Ydistance = Ydistance * -1;
                    }
                    else
                    {
                        sprintf(NS, "north");
                    }

                    if (Xdistance < 0)
                    {
                        sprintf(WE, "east");
                        Xdistance = Xdistance * -1;
                    }
                    else
                    {
                        sprintf(WE, "west");
                    }
                    sprintf(monster_string, "%c, %d %s and %d %s", d->monsters[i].to_string, Ydistance, NS, Xdistance, WE);
                }
                else
                {
                    sprintf(monster_string, "%c, dead", d->monsters[i].to_string);
                }

                for (int j = 0; j < strlen(monster_string); j++)
                {
                    if (j == 0)
                    {
                        attron(COLOR_PAIR(d->monsters[i].color));
                    }
                    if (j == 1)
                    {

                        attroff(COLOR_PAIR(d->monsters[i].color));
                    }
                    mvaddch((i % 22) + 1, j, monster_string[j]);
                }
            }

            sprintf(monster_string, "Page %d of %d", curPage, numPages);
            for (int i = 0; i < strlen(monster_string); i++)
            {
                mvaddch(23, i, monster_string[i]);
            }
            refresh();
            int ch2 = getch();
            switch (ch2)
            {
            case 27: //escape key
                done = 1;
                break;
            case 259: //up arrow
                if (curPage > 1)
                {
                    curPage--;
                }
                break;
            case 258: //down arrow
                if (curPage < numPages)
                {
                    curPage++;
                }
                break;
            }
        }

        out = 0;
        break;
    }

    case 'Q':
        clear();
        endwin();
        exit(0);
    case 'f':

        if (fog)
        {
            fog = 0;
            sprintf(header, "Disabled Fog of War");
        }
        else
        {
            fog = 1;
            sprintf(header, "Enabled Fog of War");
        }

        out = 0;
        break;
    case 'g':

        teleportPC(d);
        teleported = 1;
        sprintf(header, "Teleported pc to (%d, %d)", d->PC.pos.y, d->PC.pos.x);
        break;

    case 'w':
    {
        int slot = equip(d);
        std::string dictionary[12] = {
            "WEAPON",
            "OFFHAND",
            "RANGED",
            "ARMOR",
            "HELMET",
            "CLOAK",
            "GLOVES",
            "BOOTS",
            "AMULET",
            "LIGHT",
            "RING_1",
            "RING_2",
        };
        if (slot >= 0)
        {
            sprintf(header, "Equipped an object into the %s inventory slot", dictionary[slot].c_str());
        }

        out = 0;
        break;
    }
    case 't':
    {
        int slot = unequip(d);
        if (slot >= 0)
        {
            sprintf(header, "Unequipped an object into the %d carry slot", slot);
        }
        out = 0;
        break;
    }
    case 'd':
    {
        int slot = dropitem(d);
        if (slot >= 0)
        {
            sprintf(header, "Dropped the object in the %d carry slot onto the floor", slot);
        }
        out = 0;
        break;
    }
    case 'x':
    {
        int slot = deleteitem(d);
        if (slot >= 0)
        {
            sprintf(header, "Deleted the object in the %d carry slot", slot);
        }
        out = 0;
        break;
    }
    case 's':
    {
        int slot = sellitem(d);
        if (slot >= 0)
        {
            sprintf(header, "Sold the object in the %d carry slot", slot);
        }
        out = 0;
        break;
    }
    case 'c':
        gotoTown(d);
        out = 0;
        break;

    case 'i':
    {
        listinventory(d);
        out = 0;
        break;
    }
    case 'e':
    {
        listcarry(d);
        out = 0;
        break;
    }
    case 'I':
    {
        inspect(d);
        out = 0;
        break;
    }
    case 'L':
    {
        int returnvalue = inspectMonster(d);
        if (returnvalue == -1)
        {
            sprintf(header, "No monster there, try again");
        }
        out = 0;
        break;
    }
    default:
        sprintf(header, "Unknown Key Input: %d", ch);
        out = 0;
        break;
    }
    if ((d->PC.pos.x != xNext || d->PC.pos.y != yNext) && !teleported)
    {
        if (d->map[yNext][xNext].type == ROCK)
        {
            sprintf(header, "Unable to move, wall is there");
        }
        else
        {
            d->PC.pos.x = xNext;
            d->PC.pos.y = yNext;
        }
    }

    npc hurtmonster = d->monsters[0];

    for (int i = 0; i < d->num_monsters; i++)
    {
        if (d->monsters[i].pos.x == d->PC.pos.x && d->monsters[i].pos.y == d->PC.pos.y)
        {
            int damage = d->PC.damage.roll();

            for (int i = 0; i < 12; i++)
            {
                damage += d->PC.inventory[i].damage.roll();
            }
            d->monsters[i].hitpoints -= damage;
            if (d->monsters[i].hitpoints <= 0)
            {
                d->monsters[i].alive = 0;

                if (d->monsters[i].type[BOSS])
                {
                    win_game();
                }
            }

            //TODO if you kill the boss you win
            hurtmonster = d->monsters[i];
            break;
        }
    }
    clear();

    std::string monsterhp;
    monsterhp.push_back(hurtmonster.to_string);
    monsterhp += ": ";
    monsterhp += std::to_string(hurtmonster.hitpoints);
    monsterhp.push_back('/');
    monsterhp += std::to_string(hurtmonster.max_hitpoints);

    attron(COLOR_PAIR(hurtmonster.color));
    mvaddch(23, 0, monsterhp.at(0));
    attroff(COLOR_PAIR(hurtmonster.color));

    for (int i = 1; i < monsterhp.length(); i++)
    {

        mvaddch(23, i, monsterhp.at(i));
    }

    for (int i = 0; i < strlen(header); i++)
    {
        mvaddch(0, i, header[i]);
    }

    return out;
}

void io_init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void win_game()
{

    const char *victory =
        "\n                                       o\n"
        "                                      $\"\"$o\n"
        "                                     $\"  $$\n"
        "                                      $$$$\n"
        "                                      o \"$o\n"
        "                                     o\"  \"$\n"
        "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
        "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
        "\"oo   o o o o\n"
        "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
        "   \"o$$\"    o$$\n"
        "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
        "     o\"\"\n"
        "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
        "   o\"\n"
        "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
        "\"$$$  $\n"
        "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
        "\"\"      \"\"\" \"\n"
        "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
        "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
        "\"$$$$\n"
        "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
        "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
        "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
        "              $\"                                                 \"$\n"
        "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
        "$\"$\"$\"$\"$\"$\"$\"$\n"
        "                                   You win!\n\n";

    clear();
    int row = 0;
    int col = 0;
    for (int i = 0; i < strlen(victory); i++)
    {
        if (victory[i] == '\n')
        {
            row++;
            col = 0;
            continue;
        }
        mvaddch(row, col, victory[i]);
        col++;
    }
    refresh();
    int ch = getch();
    endwin();
    exit(0);
}

void end_game()
{
    const char *closing = "Game OVER! You died. Press any button to continue";

    for (int i = 0; i < strlen(closing); i++)
    {
        mvaddch(0, i, closing[i]);
    }
    refresh();
    int ch = getch();
    endwin();
    exit(0);
}

objects_inventory equip(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to equip (enter 0-9)");
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int carry_index;
        int ch = getch();
        object selected_object;

        switch (ch)
        {
        case '0':
            selected_object = d->PC.carry[0];
            carry_index = 0;
            break;
        case '1':
            selected_object = d->PC.carry[1];
            carry_index = 1;
            break;
        case '2':
            selected_object = d->PC.carry[2];
            carry_index = 2;
            break;
        case '3':
            selected_object = d->PC.carry[3];
            carry_index = 3;
            break;
        case '4':
            selected_object = d->PC.carry[4];
            carry_index = 4;
            break;
        case '5':
            selected_object = d->PC.carry[5];
            carry_index = 5;
            break;
        case '6':
            selected_object = d->PC.carry[6];
            carry_index = 6;
            break;
        case '7':
            selected_object = d->PC.carry[7];
            carry_index = 7;
            break;
        case '8':
            selected_object = d->PC.carry[8];
            carry_index = 8;
            break;
        case '9':
            selected_object = d->PC.carry[9];
            carry_index = 9;
            break;
        case 27: //escape key

            return NOTYPE;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object there... try again.");
            continue;
        }

        objects_inventory index = getinventoryslot(d, selected_object);

        object replaced = d->PC.inventory[index];

        d->PC.inventory[index] = selected_object;

        if (replaced.type == objtype_no_type)
        {
            d->PC.carry[carry_index] = object();
        }
        else
        {
            d->PC.carry[carry_index] = replaced;
            d->PC.hp -= replaced.hit;
        }

        d->PC.hp += selected_object.hit;

        return index;
    }
}

int unequip(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to unequip (enter a-l)");
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int carry_index;
        int ch = getch();
        object selected_object;
        objects_inventory object_unequpped;

        switch (ch)
        {
        case 'a':
            selected_object = d->PC.inventory[WEAPON];
            object_unequpped = WEAPON;
            break;
        case 'b':
            selected_object = d->PC.inventory[OFFHAND];
            object_unequpped = OFFHAND;
            break;
        case 'c':
            selected_object = d->PC.inventory[RANGED];
            object_unequpped = RANGED;
            break;
        case 'd':
            selected_object = d->PC.inventory[ARMOR];
            object_unequpped = ARMOR;
            break;
        case 'e':
            selected_object = d->PC.inventory[HELMET];
            object_unequpped = HELMET;
            break;
        case 'f':
            selected_object = d->PC.inventory[CLOAK];
            object_unequpped = CLOAK;
            break;
        case 'g':
            selected_object = d->PC.inventory[GLOVES];
            object_unequpped = GLOVES;
            break;
        case 'h':
            selected_object = d->PC.inventory[BOOTS];
            object_unequpped = BOOTS;
            break;
        case 'i':
            selected_object = d->PC.inventory[AMULET];
            object_unequpped = AMULET;
            break;
        case 'j':
            selected_object = d->PC.inventory[LIGHT];
            object_unequpped = LIGHT;
            break;
        case 'k':
            selected_object = d->PC.inventory[RING_1];
            object_unequpped = RING_1;
            break;
        case 'l':
            selected_object = d->PC.inventory[RING_2];
            object_unequpped = RING_2;
            break;
        case 27: //escape key

            return NOTYPE;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object to unequip, try again");
            continue;
        }

        for (int i = 0; i < 10; i++)
        {
            if (d->PC.carry[i].type == objtype_no_type)
            {
                d->PC.carry[i] = selected_object;
                d->PC.hp -= selected_object.hit;
                d->PC.inventory[object_unequpped] = object();
                return i;
            }
        }

        return -1;
    }
}

int deleteitem(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to delete (enter 0-9)");
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int carry_index;
        int ch = getch();
        object selected_object;

        switch (ch)
        {
        case '0':
            selected_object = d->PC.carry[0];
            carry_index = 0;
            break;
        case '1':
            selected_object = d->PC.carry[1];
            carry_index = 1;
            break;
        case '2':
            selected_object = d->PC.carry[2];
            carry_index = 2;
            break;
        case '3':
            selected_object = d->PC.carry[3];
            carry_index = 3;
            break;
        case '4':
            selected_object = d->PC.carry[4];
            carry_index = 4;
            break;
        case '5':
            selected_object = d->PC.carry[5];
            carry_index = 5;
            break;
        case '6':
            selected_object = d->PC.carry[6];
            carry_index = 6;
            break;
        case '7':
            selected_object = d->PC.carry[7];
            carry_index = 7;
            break;
        case '8':
            selected_object = d->PC.carry[8];
            carry_index = 8;
            break;
        case '9':
            selected_object = d->PC.carry[9];
            carry_index = 9;
            break;
        case 27: //escape key

            return NOTYPE;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object there... try again.");
            continue;
        }

        d->PC.carry[carry_index] = object();

        return carry_index;
    }
}

int sellitem(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to sell? (enter 0-9)");
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int carry_index;
        int ch = getch();
        object selected_object;

        switch (ch)
        {
        case '0':
            selected_object = d->PC.carry[0];
            carry_index = 0;
            break;
        case '1':
            selected_object = d->PC.carry[1];
            carry_index = 1;
            break;
        case '2':
            selected_object = d->PC.carry[2];
            carry_index = 2;
            break;
        case '3':
            selected_object = d->PC.carry[3];
            carry_index = 3;
            break;
        case '4':
            selected_object = d->PC.carry[4];
            carry_index = 4;
            break;
        case '5':
            selected_object = d->PC.carry[5];
            carry_index = 5;
            break;
        case '6':
            selected_object = d->PC.carry[6];
            carry_index = 6;
            break;
        case '7':
            selected_object = d->PC.carry[7];
            carry_index = 7;
            break;
        case '8':
            selected_object = d->PC.carry[8];
            carry_index = 8;
            break;
        case '9':
            selected_object = d->PC.carry[9];
            carry_index = 9;
            break;
        case 27: //escape key

            return NOTYPE;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object there... try again.");
            continue;
        }

        d->PC.carry[carry_index] = object();
        d->PC.money += selected_object.value;

        return carry_index;
    }
}

int dropitem(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to drop (enter 0-9)");
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int carry_index;
        int ch = getch();
        object selected_object;

        switch (ch)
        {
        case '0':
            selected_object = d->PC.carry[0];
            carry_index = 0;
            break;
        case '1':
            selected_object = d->PC.carry[1];
            carry_index = 1;
            break;
        case '2':
            selected_object = d->PC.carry[2];
            carry_index = 2;
            break;
        case '3':
            selected_object = d->PC.carry[3];
            carry_index = 3;
            break;
        case '4':
            selected_object = d->PC.carry[4];
            carry_index = 4;
            break;
        case '5':
            selected_object = d->PC.carry[5];
            carry_index = 5;
            break;
        case '6':
            selected_object = d->PC.carry[6];
            carry_index = 6;
            break;
        case '7':
            selected_object = d->PC.carry[7];
            carry_index = 7;
            break;
        case '8':
            selected_object = d->PC.carry[8];
            carry_index = 8;
            break;
        case '9':
            selected_object = d->PC.carry[9];
            carry_index = 9;
            break;
        case 27: //escape key

            return NOTYPE;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object there... try again.");
            continue;
        }

        d->PC.carry[carry_index] = object();
        int dungeon_index = selected_object.position_in_dungeon;

        int x = d->PC.pos.x;
        int y = d->PC.pos.y;

        for (int i = y - 1; i < y + 2; i++)
        {
            for (int j = x - 1; j < x + 2; j++)
            {
                if (i == y && j == x)
                {
                    continue;
                }

                if (d->map[i][j].type == ROOM)
                {
                    d->objects[dungeon_index].picked_up = false;

                    d->objects[dungeon_index].pos.x = j;
                    d->objects[dungeon_index].pos.y = i;
                    return carry_index;
                }
            }
        }

        return -1;
    }
}

void inspect(dungeon_type *d)
{
    char header[100];

    sprintf(header, "What item would you like to inspect (enter 0-9)");
    object selected_object;
    int carry_index;
    while (1)
    {
        for (int i = 0; i < 79; i++)
        {
            mvaddch(0, i, ' ');
        }

        for (int i = 0; i < strlen(header); i++)
        {
            mvaddch(0, i, header[i]);
        }

        refresh();

        int ch = getch();

        switch (ch)
        {
        case '0':
            selected_object = d->PC.carry[0];
            carry_index = 0;
            break;
        case '1':
            selected_object = d->PC.carry[1];
            carry_index = 1;
            break;
        case '2':
            selected_object = d->PC.carry[2];
            carry_index = 2;
            break;
        case '3':
            selected_object = d->PC.carry[3];
            carry_index = 3;
            break;
        case '4':
            selected_object = d->PC.carry[4];
            carry_index = 4;
            break;
        case '5':
            selected_object = d->PC.carry[5];
            carry_index = 5;
            break;
        case '6':
            selected_object = d->PC.carry[6];
            carry_index = 6;
            break;
        case '7':
            selected_object = d->PC.carry[7];
            carry_index = 7;
            break;
        case '8':
            selected_object = d->PC.carry[8];
            carry_index = 8;
            break;
        case '9':
            selected_object = d->PC.carry[9];
            carry_index = 9;
            break;
        case 27: //escape key

            return;
        default:
            sprintf(header, "Unknown Key Input: %d", ch);
            continue;
            break;
        }

        if (selected_object.type == objtype_no_type)
        {
            sprintf(header, "No object there... try again.");
            continue;
        }

        break;
    }

    int done = 0;
    while (!done)
    {
        char monster_string[100];
        sprintf(monster_string, "Description:");
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }

        for (int i = 0; i < selected_object.description.length(); i++)
        {
            int row = (i / 80) + 1;
            mvaddch(row, i % 80, selected_object.description.at(i));
        }

        refresh();
        int ch2 = getch();

        if (ch2 == 27)
        {
            done = 1;
        }
    }
}

void gotoTown(dungeon_type *d)
{

    const char *town =
        "                                                            _|_\n"
        "                                                             |\n"
        "                                                            )A(\n"
        "       ^ _______________________________                    | |\n"
        "       [=U=U=U=U=U=U=U=U=U=U=U=U=U=U=U=]               ,  _+|_|+_  ,\n"
        "       |.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.|              )A( |:|:|:| )A(\n"
        "       |      +-+-+-+-+-+-+-+-+-+      |              | |---------| |\n"
        "       |      |Armor and Weapons|      |         ,    | | Thielen | |    ,\n"
        "       |      +-+-+-+-+-+-+-+-+-+      |        )A(_+_| |---------| |_+_)A(\n"
        "       |.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.|        | |:|:| |:|:|:|:|:| |:|:| |\n"
        "       |  _________  __ __  _________  |        | ||H|| ||H|H|H|H|| ||H|| |\n"
        "     _ | |___   _  ||[]|[]||  _      | | _      | |:|:| |:|:|:|:|:| |:|:| |\n"
        "    (!)||OPEN|_(!)_|| ,| ,||_(!)_____| |(!)     |_||H||_||H|===|H||_||H||_|\n"
        "   .T~T|:.....:T~T.:|__|__|:.T~T.:....:|T~T.  ~^^\"      ~^^/   \\^^~      \"^^~   \n"
        "--------------------------------------------------------------------------------  \n"
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ |= _ _:_ _ =| _ _ _ _ _ _ _ _ _ _ _ _ _   \n"
        "                                        |=    :    =|                             \n"
        "________________________________________L___________J___________________________  \n"
        "--------------------------------------------------------------------------------";
    clear();
    int row = 0;
    int col = 0;
    bool done = false;
    position pc;
    pc.x = 42;
    pc.y = 15;

    while (!done)
    {

        std::string header = "Welcome to the town! You can walk into either shop, or press escape!";

        for (int i = 0; i < header.length(); i++)
        {
            mvaddch(0, i, header.at(i));
        }
        int row = 1;
        int col = 0;
        for (int i = 0; i < strlen(town); i++)
        {
            if (town[i] == '\n')
            {
                row++;
                col = 0;
                continue;
            }
            mvaddch(row, col, town[i]);
            col++;
        }

        int maxhealth = 200;
        for (int i = 0; i < 12; i++)
        {
            d->PC.speed += d->PC.inventory[i].speed;
            maxhealth += d->PC.inventory[i].hit;
        }

        std::string hp = "Player Health: ";
        hp += std::to_string(d->PC.hp);
        hp.push_back('/');
        hp += std::to_string(maxhealth);

        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        std::string coins = "Player Money: ";
        coins += std::to_string(d->PC.money);
        coins += " coins";
        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        for (int i = 80 - coins.length(); i < 80; i++)
        {
            mvaddch(22, i, coins.at(i - (80 - coins.length())));
        }

        mvaddch(pc.y, pc.x, '@');
        refresh();
        int ch = getch();

        uint8_t yNext = pc.y;
        uint8_t xNext = pc.x;

        switch (ch)
        {
        case '7':
        case 'y':
            yNext--;
            xNext--;
            break;
        case '8':
        case 'k':
            yNext--;
            break;
        case '9':
        case 'u':

            yNext--;
            xNext++;
            break;
        case '6':
        case 'l':

            xNext++;
            break;
        case '3':
        case 'n':
            yNext++;
            xNext++;
            break;
        case '2':
        case 'j':
            yNext++;
            break;
        case '1':
        case 'b':
            yNext++;
            xNext--;
            break;
        case '4':
        case 'h':
            xNext--;
            break;
        case 27: //escape key

            return;
        }

        //armor col: 20-26
        //health 59-63
        if (yNext >= 14 && yNext < 19 && xNext >= 0 && xNext < 80)
        {
            pc.x = xNext;
            pc.y = yNext;
        }

        if (pc.y == 14)
        {
            if (pc.x >= 20 && pc.x <= 26)
            {
                armorShop(d);
                pc.y = 15;
            }

            if (pc.x >= 59 && pc.x <= 63)
            {
                visithealthcenter(d);
                pc.y = 15;
            }
        }
    }
}

void armorShop(dungeon_type *d)
{

    object *objects = new object[3];

    for (int i = 0; i < 3; i++)
    {
        objects[i] = object();

        object_description desc;

        desc = d->object_descriptions.at(rand() % (d->object_descriptions.size()));
        position pos;
        pos.x = 0;
        pos.y = 0;

        objects[i].set(desc.name, desc.description, desc.type, desc.color, desc.hit.roll(), desc.damage, desc.dodge.roll(), desc.defence.roll(), desc.weight.roll(), desc.speed.roll(), desc.attribute.roll(), desc.value.roll(), desc.artifact, desc.rarity, pos, true, -1);
    }

    char monster_string[100];
    sprintf(monster_string, "Welcome to the Armor and weapons store! Press 1-3 to buy");
    clear();

    int done = 0;
    while (!done)
    {
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }
        for (int i = 0; i < 3; i++)
        {
            sprintf(monster_string, "[%d]: %c, %s COST: %d", i + 1, object_symbol[d->objects[i].type], objects[i].name.c_str(), objects[i].value);

            for (int j = 0; j < strlen(monster_string); j++)
            {
                if (j == 5)
                {
                    attron(COLOR_PAIR(objects[i].color));
                    mvaddch(i + 1, j, monster_string[j]);
                    attroff(COLOR_PAIR(objects[i].color));
                    continue;
                }
                mvaddch(i + 1, j, monster_string[j]);
            }
        }

        int maxhealth = 200;
        for (int i = 0; i < 12; i++)
        {
            d->PC.speed += d->PC.inventory[i].speed;
            maxhealth += d->PC.inventory[i].hit;
        }

        std::string hp = "Player Health: ";
        hp += std::to_string(d->PC.hp);
        hp.push_back('/');
        hp += std::to_string(maxhealth);

        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        std::string coins = "Player Money: ";
        coins += std::to_string(d->PC.money);
        coins += " coins";
        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        for (int i = 80 - coins.length(); i < 80; i++)
        {
            mvaddch(22, i, coins.at(i - (80 - coins.length())));
        }

        refresh();
        int ch2 = getch();
        object selectedobject;
        switch (ch2)
        {
        case '1':
            selectedobject = objects[0];
            break;
        case '2':
            selectedobject = objects[1];
            break;
        case '3':
            selectedobject = objects[2];
            break;
        case 27:
            done = 1;
            break;
        default:
            sprintf(monster_string, "Unknown Key Input: %d", ch2);
            continue;
            break;
        }

        int leftover_money = d->PC.money - selectedobject.value;
        if (leftover_money < 0)
        {
            sprintf(monster_string, "You do not have enough money!");
            continue;
        }

        int slot;
        bool full = false;
        for (int i = 0; i < 10; i++)
        {
            if (d->PC.carry[i].type == objtype_no_type)
            {
                d->PC.carry[i] = selectedobject;
                slot = i;
                break;
            }
            if (i == 9)
            {
                full = true;
            }
        }

        if (!full)
        {
            d->PC.money = leftover_money;
            sprintf(monster_string, "%s placed into carry slot #%d", selectedobject.name.c_str(), slot);
        }
        else
        {
            sprintf(monster_string, "Unable to purcase %s, inventory is full", selectedobject.name.c_str());
        }
    }
}

void visithealthcenter(dungeon_type *d)
{

    char monster_string[100];
    sprintf(monster_string, "Welcome to Thielen! Press 1-3 to buy");
    clear();

    const std::string potion[3] = {"[1]: Minor health potion COST: 500", "[2]: Major health potion COST: 750", "[3]: Clone Cone COST: 1000"};

    int done = 0;
    while (!done)
    {
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }
        for (int i = 0; i < 3; i++)
        {

            for (int j = 0; j < potion[i].length(); j++)
            {
                mvaddch(i + 1, j, potion[i].at(j));
            }
        }

        int maxhealth = 200;
        for (int i = 0; i < 12; i++)
        {
            d->PC.speed += d->PC.inventory[i].speed;
            maxhealth += d->PC.inventory[i].hit;
        }

        std::string hp = "Player Health: ";
        hp += std::to_string(d->PC.hp);
        hp.push_back('/');
        hp += std::to_string(maxhealth);

        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        std::string coins = "Player Money: ";
        coins += std::to_string(d->PC.money);
        coins += " coins";
        for (int i = 0; i < hp.length(); i++)
        {
            mvaddch(22, i, hp.at(i));
        }

        for (int i = 80 - coins.length(); i < 80; i++)
        {
            mvaddch(22, i, coins.at(i - (80 - coins.length())));
        }
        int cost;
        int heal;
        refresh();
        int ch2 = getch();
        switch (ch2)
        {
        case '1':
            cost = 500;
            heal = 50;
            break;
        case '2':
            cost = 750;
            heal = 100;
            break;
        case '3':
            cost = 1000;
            heal = 9999999;
            break;
        case 27:
            done = 1;
            break;
        default:
            sprintf(monster_string, "Unknown Key Input: %d", ch2);
            continue;
            break;
        }

        int leftover_money = d->PC.money - cost;
        if (leftover_money < 0)
        {
            sprintf(monster_string, "You do not have enough money!");
            continue;
        }

        if(d->PC.hp == maxhealth){
            sprintf(monster_string, "You are already at max health!");
            continue;
        }

        d->PC.money = leftover_money;

        int oldHP = d->PC.hp;

        d->PC.hp = std::min(maxhealth, d->PC.hp + heal);

        sprintf(monster_string, "Healed the player %d points", d->PC.hp - oldHP);
    }
}

void listinventory(dungeon_type *d)
{

    int done = 0;
    while (!done)
    {
        char monster_string[100];
        sprintf(monster_string, "Inventory:");
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }
        for (int i = 0; i < 12; i++)
        {
            std::string dictionaryalpha = "abcdefghijkl";
            std::string dictionary[12] = {
                "WEAPON",
                "OFFHAND",
                "RANGED",
                "ARMOR",
                "HELMET",
                "CLOAK",
                "GLOVES",
                "BOOTS",
                "AMULET",
                "LIGHT",
                "RING_1",
                "RING_2",
            };

            if (d->PC.inventory[i].type != objtype_no_type)
            {
                sprintf(monster_string, "%s (%c): %s", dictionary[i].c_str(), dictionaryalpha.at(i), d->PC.inventory[i].name.c_str());
            }
            else
            {
                sprintf(monster_string, "%s (%c): nothing", dictionary[i].c_str(), dictionaryalpha.at(i));
            }

            for (int j = 0; j < strlen(monster_string); j++)
            {

                mvaddch(i + 1, j, monster_string[j]);
            }
        }

        refresh();
        int ch2 = getch();

        if (ch2 == 27)
        {
            done = 1;
        }
    }
}

void listcarry(dungeon_type *d)
{

    int done = 0;
    while (!done)
    {
        char monster_string[100];
        sprintf(monster_string, "Carrying:");
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }
        for (int i = 0; i < 10; i++)
        {
            std::string dictionaryalpha = "0123456789";
            std::string dictionary[12] = {
                "WEAPON",
                "OFFHAND",
                "RANGED",
                "ARMOR",
                "HELMET",
                "CLOAK",
                "GLOVES",
                "BOOTS",
                "AMULET",
                "LIGHT",
                "RING_1",
                "RING_2",
            };

            if (d->PC.carry[i].type != objtype_no_type)
            {
                int typeindex = getinventoryslot(d, d->PC.carry[i]);
                sprintf(monster_string, "%c: %s (%s)", dictionaryalpha.at(i), d->PC.carry[i].name.c_str(), dictionary[typeindex].c_str());
            }
            else
            {
                sprintf(monster_string, "%c: nothing", dictionaryalpha.at(i));
            }

            for (int j = 0; j < strlen(monster_string); j++)
            {

                mvaddch(i + 1, j, monster_string[j]);
            }
        }

        refresh();
        int ch2 = getch();

        if (ch2 == 27)
        {
            done = 1;
        }
    }
}

objects_inventory getinventoryslot(dungeon_type *d, object o)
{

    switch (o.type)
    {

    case objtype_WEAPON:
        return WEAPON;
    case objtype_OFFHAND:
        return OFFHAND;
    case objtype_RANGED:
        return RANGED;
    case objtype_ARMOR:
        return ARMOR;
    case objtype_HELMET:
        return HELMET;
    case objtype_CLOAK:
        return CLOAK;
    case objtype_GLOVES:
        return GLOVES;
    case objtype_BOOTS:
        return BOOTS;
    case objtype_AMULET:
        return AMULET;
    case objtype_LIGHT:
        return LIGHT;
    case objtype_RING:
        if (d->PC.inventory[RING_1].type == d->PC.inventory[RING_2].type)
        {
            return RING_1;
        }
        else
        {
            return RING_2;
        }
    default:
        return NOTYPE;
    }
}

void teleportPC(dungeon_type *d)
{
    position newPos;
    newPos.x = d->PC.pos.x;
    newPos.y = d->PC.pos.y;

    while (1)
    {

        printDungeon(d);
        mvaddch(newPos.y + 1, newPos.x, '*');
        refresh();
        int ch = getch();

        uint8_t yNext = newPos.y;
        uint8_t xNext = newPos.x;
        int out = 0;

        switch (ch)
        {
        case '7':
        case 'y':
            yNext--;
            xNext--;
            break;
        case '8':
        case 'k':
            yNext--;
            break;
        case '9':
        case 'u':

            yNext--;
            xNext++;
            break;
        case '6':
        case 'l':

            xNext++;
            break;
        case '3':
        case 'n':
            yNext++;
            xNext++;
            break;
        case '2':
        case 'j':
            yNext++;
            break;
        case '1':
        case 'b':
            yNext++;
            xNext--;
            break;
        case '4':
        case 'h':
            xNext--;
            break;
        case 'g':
            out = 1;
            break;
        case 'r':

            newPos.x = rand() % (DUNGEON_X - 1) + 1;
            newPos.y = rand() % (DUNGEON_Y - 1) + 1;
            out = 1;
            break;
        }

        if (out)
        {
            break;
        }

        if ((xNext >= 0 && xNext < DUNGEON_X) && (yNext >= 0 && yNext < DUNGEON_Y) && d->map[yNext][xNext].hardness != 255)
        {
            newPos.x = xNext;
            newPos.y = yNext;
        }
    }

    d->PC.pos.x = newPos.x;
    d->PC.pos.y = newPos.y;
}

int inspectMonster(dungeon_type *d)
{
    position newPos;
    newPos.x = d->PC.pos.x;
    newPos.y = d->PC.pos.y;

    while (1)
    {

        printDungeon(d);
        mvaddch(newPos.y + 1, newPos.x, '*');
        refresh();
        int ch = getch();

        uint8_t yNext = newPos.y;
        uint8_t xNext = newPos.x;
        int out = 0;

        switch (ch)
        {
        case '7':
        case 'y':
            yNext--;
            xNext--;
            break;
        case '8':
        case 'k':
            yNext--;
            break;
        case '9':
        case 'u':

            yNext--;
            xNext++;
            break;
        case '6':
        case 'l':

            xNext++;
            break;
        case '3':
        case 'n':
            yNext++;
            xNext++;
            break;
        case '2':
        case 'j':
            yNext++;
            break;
        case '1':
        case 'b':
            yNext++;
            xNext--;
            break;
        case '4':
        case 'h':
            xNext--;
            break;
        case 't':
            out = 1;
            break;
        case 27: //escape key

            return 0;
        }

        if (out)
        {
            break;
        }

        if ((xNext >= 0 && xNext < DUNGEON_X) && (yNext >= 0 && yNext < DUNGEON_Y) && d->map[yNext][xNext].hardness != 255)
        {
            newPos.x = xNext;
            newPos.y = yNext;
        }
    }
    npc selected_monster = npc();

    for (int i = 0; i < d->num_monsters; i++)
    {
        if (d->monsters[i].pos.x == newPos.x && d->monsters[i].pos.y == newPos.y)
        {
            selected_monster = d->monsters[i];
        }
    }

    if (selected_monster.rarity == 0)
    {
        return -1;
    }

    int done = 0;
    while (!done)
    {
        char monster_string[100];
        sprintf(monster_string, "Name: %s", selected_monster.name.c_str());
        clear();
        for (int i = 0; i < strlen(monster_string); i++)
        {
            mvaddch(0, i, monster_string[i]);
        }

        std::string symbol = "Symbol: ";
        symbol.push_back(selected_monster.to_string);

        for (int i = 0; i < symbol.length(); i++)
        {
            if (i == symbol.length() - 1)
            {

                attron(COLOR_PAIR(selected_monster.color));
                mvaddch(1, i, symbol.at(i));
                attroff(COLOR_PAIR(selected_monster.color));
            }
            else
            {
                mvaddch(1, i, symbol.at(i));
            }
        }

        std::string description = "Description: " + selected_monster.description;

        for (int i = 0; i < description.length(); i++)
        {
            int row = (i / 80) + 2;
            mvaddch(row, i % 80, description.at(i));
        }

        refresh();
        int ch2 = getch();

        if (ch2 == 27)
        {
            done = 1;
        }
    }

    return 1;
}

void printFogDungeon(dungeon_type *d)
{

    for (int i = 0; i < DUNGEON_Y; i++)
    {

        for (int j = 0; j < DUNGEON_X; j++)
        {

            if (d->PC.pos.y == i && d->PC.pos.x == j)
            {
                mvaddch(i + 1, j, '@');
            }
            else if (d->PC.map[i][j].type == ROCK)
            {
                mvaddch(i + 1, j, ' ');
            }
            else if (d->PC.map[i][j].type == ROOM)
            {
                mvaddch(i + 1, j, '.');
                for (int k = 0; k < d->num_objects; k++)
                {
                    if (d->objects[k].pos.y == i && d->objects[k].pos.x == j && d->objects[k].picked_up == false)
                    {
                        char object_char = object_symbol[d->objects[k].type];
                        attron(COLOR_PAIR(d->objects[k].color));
                        mvaddch(i + 1, j, object_char);
                        attroff(COLOR_PAIR(d->objects[k].color));
                        break;
                    }
                }
            }
            else if (d->PC.map[i][j].type == CORRIDOR)
            {
                mvaddch(i + 1, j, '#');
            }
            else if (d->PC.map[i][j].type == UP)
            {
                mvaddch(i + 1, j, '<');
            }
            else if (d->PC.map[i][j].type == DOWN)
            {
                mvaddch(i + 1, j, '>');
            }
        }
    }

    for (int i = 0; i < nummon; i++)
    {
        int xDis = std::abs(d->monsters[i].pos.x - d->PC.pos.x);
        int yDis = std::abs(d->monsters[i].pos.y - d->PC.pos.y);

        if (xDis < 3 && yDis < 3)
        {
            if (d->monsters[i].alive == 1 && islineofsight(d, d->monsters[i].pos.y, d->monsters[i].pos.x, d->PC.pos))
            {
                attron(COLOR_PAIR(d->monsters[i].color));
                mvaddch(d->monsters[i].pos.y + 1, d->monsters[i].pos.x, d->monsters[i].to_string);
                attroff(COLOR_PAIR(d->monsters[i].color));
            }
        }
    }
}
