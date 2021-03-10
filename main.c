#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "dungeondefinitions.h"

heap_t h;
dungeon_type dungeon;
int nummon;

int player_turn(dungeon_type *d);
void io_init_terminal(void);
void end_game();

int main(int argc, char const *argv[])
{
    io_init_terminal();

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

    if (boolload == 1)
    {
        load();
    }
    else
    {
        generate();
    }

    dungeon.PC.alive = 1;
    dungeon.PC.nextturn = 0;
    dungeon.PC.sequencenumber = 0;
    dungeon.PC.speed = 10;
    dungeon.PC.to_string = '@';
    dungeon.PC.isNPC = 0;

    dungeon_type *d;
    d = &dungeon;

    if (boolsave == 1)
    {
        writetodisk();
    }

    h = init_monsters(d, nummon);
    char *header = "Welcome to David Bone's cs327 project!";
    for (int i = 0; i < strlen(header); i++)
    {
        mvaddch(0, i, header[i]);
    }
    run_game(d);
}

void run_game(dungeon_type *d)
{

    static monster *c;
    while ((c = heap_remove_min(&h)))
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
    printDungeon(d);

    char *placeholder = "placeholder";
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
            dungeon = generate();
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
            generate();
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

                for (int j = 0; j < strlen(monster_string); j++)
                {
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
    default:
        sprintf(header, "Unknown Key Input: %d", ch);
        out = 0;
        break;
    }
    if (d->PC.pos.x != xNext || d->PC.pos.y != yNext)
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
    char *closing = "Game OVER! You died. Press any button to continue";

    for (int i = 0; i < strlen(closing); i++)
    {
        mvaddch(0, i, closing[i]);
    }
    refresh();
    int ch = getch();
    endwin();
    exit(0);
}
