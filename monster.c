#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "dungeondefinitions.h"

char *hextobinary(int num);

int islineofsight(dungeon_type *d, monster m, position pc);

void movemonsternontunneling(dungeon_type *d, monster m);

void init_monsters(dungeon_type *d, int numMonsters)
{
    int seed = 1614733845; //(unsigned) time(NULL);
    srand(seed);
    printf("monster seed: %d\n", seed);
    monster *monsters = malloc(numMonsters * sizeof(monster));

    for (int i = 0; i < numMonsters; i++)
    {
        monsters[i].speed = (rand() % 16) + 5;
        int type = rand() % 16;
        monsters[i].type = hextobinary(type);
        monsters[i].alive = 1;
        monsters[i].sequencenumber = i + 1;
        monsters[i].nextturn = 0;
        monsters[i].memory.y = 0;
        monsters[i].memory.x = 0;

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

void movemonsternontunneling(dungeon_type *d, monster m)
{
    uint8_t nextY = m.y;
    uint8_t nextX = m.x;

    if (atoi(&m.type[INTELLIGENCE]) && atoi(&m.type[TELEPATHY]))
    {
        int **path = nontunnel_path_finder(d, d->PC.pos.y, d->PC.pos.x);

        for (int y = m.y - 1; y <= m.y + 1; y++)
        {
            for (int x = m.x - 1; x <= m.x + 1; x++)
            {
                if (y == m.y && x == m.x)
                {
                    continue;
                }
                if (path[y][x] < path[m.y][m.x])
                {

                    nextY = y;
                    nextX = x;
                }
            }
        }
    }
    else if (atoi(&m.type[TELEPATHY]))
    {
    }
    else if (atoi(&m.type[INTELLIGENCE]))
    {
    }
    else
    {

        if (islineofsight(d, m, d->PC.pos))
        {
            m.memory.y = d->PC.pos.y;
            m.memory.x = d->PC.pos.x;
        }

        if (m.memory.y && m.memory.y)
        {

            double shortestdistance = sqrt(pow(m.y - m.memory.y, 2) + pow(m.x - m.memory.x, 2));
            double distance;

            int shortestY = m.y;
            int shortestX = m.x;

            for (int i = m.y - 1; i < m.y + 2; i++)
            {
                for (int j = m.x - 1; j < m.x + 2; j++)
                {
                    if (i == m.y && j == m.x)
                    {
                        continue;
                    }

                    distance = sqrt(pow((i)-m.memory.y, 2) + pow((j)-m.memory.x, 2));
                    if (shortestdistance > distance)
                    {
                        shortestY = i;
                        shortestX = j;
                        shortestdistance = distance;
                    }
                }
            }

            if (d->map[shortestY][shortestX].type == ROOM || d->map[shortestY][shortestX].type == CORRIDOR)
            {
                nextY = shortestY;
                nextX = shortestX;
            }
        }
    }

    if (atoi(&m.type[ERRATIC]))
    {
        int chance = rand() % 2;

        if (chance)
        {
            while (1)
            {
                int randY, randX;
                randY = (rand() % 3) + (m.y - 1);
                randX = (rand() % 3) + (m.x - 1);
                if (randY == m.y && randX == m.x)
                {
                    continue;
                }

                if (d->map[randY][randX].type == ROOM || d->map[randY][randX].type == CORRIDOR)
                {
                    nextY = randY;
                    nextX = randX;
                    break;
                }
            }
        }
    }

    if (m.y != nextY || m.x != nextX)
    {
        for (int i = 0; i < d->num_monsters; i++)
        {
            if (d->monsters[i].y == nextY && d->monsters[i].x == nextX)
            {
                d->monsters[i].alive = 0;
            }
            if (d->monsters[i].y == m.y && d->monsters[i].x == m.x)
            {
                d->monsters[i].y = nextY;
                d->monsters[i].x = nextX;
            }
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

int islineofsight(dungeon_type *d, monster m, position pc)
{

    uint8_t y = m.y;
    uint8_t x = m.x;

    while (1)
    {
        double shortestdistance = sqrt(pow(y - pc.y, 2) + pow(x - pc.x, 2));
        double distance;

        uint8_t yNext = y;
        uint8_t xNext = x;

        for (int i = y - 1; i < y + 2; i++)
        {
            for (int j = x - 1; j < x + 2; j++)
            {
                if (i == y && j == x)
                {
                    continue;
                }

                distance = sqrt(pow((i)-pc.y, 2) + pow((j)-pc.x, 2));
                if (shortestdistance > distance)
                {
                    yNext = i;
                    xNext = j;
                    shortestdistance = distance;
                }
            }
        }

        if (d->map[yNext][xNext].type == ROCK)
        {
            return 0;
        }

        y = yNext;
        x = xNext;

        if (y == pc.y && x == pc.x)
        {
            break;
        }
    }
    return 1;
}
