#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "dungeondefinitions.h"

char *hextobinary(int num);

void init_monsters(dungeon_type *d, int numMonsters)
{

    srand((unsigned)time(NULL));

    monster *monsters = malloc(numMonsters * sizeof(monster));

    for (int i = 0; i < numMonsters; i++)
    {
        monsters[i].speed = (rand() % 16) + 5;
        int type = rand() % 16;
        monsters[i].type = hextobinary(type);

        if (type < 10)
        {
            char numeric[] = "0123456789";
            monsters[i].to_string = numeric[type];
        }
        else
        {
            char alpha[] = "abcdef";
            monsters[i].to_string = alpha[type - 10];
        }

        while (1)
        {
            int Y = (rand() % (DUNGEON_Y - 1)) + 1;
            int X = (rand() % (DUNGEON_X - 1)) + 1;
            if (d->map[Y][X].type == ROOM || d->map[Y][X].type == CORRIDOR)
            {
                monsters[i].y = Y;
                monsters[i].x = X;
                break;
            }
        }
    }

    d->monsters = monsters;
    d->num_monsters = numMonsters;
}


void movemonsternontunneling(dungeon_type *d, monster m){
    uint8_t nextY, nextX;

    if(m.type[INTELLIGENCE] && m.type[TELEPATHY]){
        path_data path[DUNGEON_Y][DUNGEON_X] = tunnel_path_finder(d, d->PC.y, d->PC.x);

        for (int y = m.y - 1; y <= m.y + 1; y++)
        {
            for (int x = m.x - 1; x <= m.x + 1; x++)
            {
                if (y == m.y && x == m.x)
                {
                    continue;
                }
                if(path[y][x].cost < path[m.y][m.x].cost){

                    nextY = y;
                    nextX = x;
                }

            }
        }
    }




    if(m.type[ERRATIC]){
        int chance = rand()%2;

        if(chance){

        }
    }



}

char *hextobinary(int num)
{

    switch (num)
    {
    case 0:
        return "0000";
        break;

    case 1:
        return "0001";
        break;

    case 2:
        return "0010";
        break;

    case 3:
        return "0011";
        break;

    case 4:
        return "0100";
        break;

    case 5:
        return "0101";
        break;

    case 6:
        return "0110";
        break;

    case 7:
        return "0111";
        break;
    case 8:
        return "1000";
        break;

    case 9:
        return "1001";
        break;

    case 10:
        return "1010";
        break;

    case 11:
        return "1011";
        break;

    case 12:
        return "1100";
        break;

    case 13:
        return "1101";
        break;

    case 14:
        return "1110";
        break;

    case 15:
        return "1111";
        break;
    }
    return "0000";
}
