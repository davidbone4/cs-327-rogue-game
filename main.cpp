#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <cmath>

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
            while (!out)
            {
                out = player_turn(d);
            }

            if (c->alive == 0)
            {
                end_game();
                break;
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

    const char *placeholder = "placeholder";
    for (int i = 0; i < strlen(placeholder); i++)
    {
        mvaddch(22, i, placeholder[i]);
    }
    for (int i = 0; i < strlen(placeholder); i++)
    {
        mvaddch(23, i, placeholder[i]);
    }

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

    clear();

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
        }

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
                d->PC.inventory[object_unequpped] = object();
                return i;
            }
        }

        return -1;
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
            if (islineofsight(d, d->monsters[i].pos.y, d->monsters[i].pos.x, d->PC.pos))
            {
                attron(COLOR_PAIR(d->monsters[i].color));
                mvaddch(d->monsters[i].pos.y + 1, d->monsters[i].pos.x, d->monsters[i].to_string);
                attroff(COLOR_PAIR(d->monsters[i].color));
            }
        }
    }
}
