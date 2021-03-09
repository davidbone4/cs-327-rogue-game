#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <ncurses.h>

#include "dungeondefinitions.h"

heap_t h;
dungeon_type dungeon;

void player_turn(dungeon_type *d);
void io_init_terminal(void);

int main(int argc, char const *argv[])
{
    io_init_terminal();

    int boolsave = 0;
    int boolload = 0;
    int nummon = 10;
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
    run_game(d);
}

void run_game(dungeon_type *d)
{

    static monster *c;
    while ((c = heap_remove_min(&h)))
    {

        if (c->isNPC == 0)
        {

            player_turn(d);
            if (c->alive == 0)
            {
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

    char *closing = "Game OVER! You died. Press any button to continue";

    for (int i = 0; i < strlen(closing); i++)
    {
        mvaddch(0, i, closing[i]);
    }
    refresh();
    int ch = getch();
    endwin();
}

void player_turn(dungeon_type *d)
{

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

    clear();

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
        default:
            sprintf(header, "The key you have pressed is %d", ch);
            break;
    }

    // if(d->map[yNext][xNext].type == ROCK){
    //     sprintf(header, "Unable to move, wall is there");
    // }else{
    //     d->PC.pos.x = xNext;
    //     d->PC.pos.x = yNext;
    // }

    //TODO: weird error where pc moves very far and stays there

    for(int i = 0; i < strlen(header); i++){
        mvaddch(0,i,header[i]);
    }

}

void io_init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}
