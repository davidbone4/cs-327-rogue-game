#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include "dungeondefinitions.h"

extern dungeon_type dungeon;

void printDungeon(dungeon_type *d);

dungeon_type generate(int seed);

int load();

int writetodisk();

dungeon_type generate(int seed)
{

    srand(seed);
    // printf("generate seed: %d\n", seed);

    int numRooms = (rand() % (MAX_ROOMS - MIN_ROOMS + 1)) + MIN_ROOMS;
    dungeon.num_rooms = numRooms;

    struct room8bit rooms[numRooms];

    while (1)
    {

        for (int i = 0; i < DUNGEON_Y; i++)
        {
            for (int j = 0; j < DUNGEON_X; j++)
            {
                dungeon.map[i][j].type = ROCK;
                if (i == 0 || i == DUNGEON_Y - 1 || j == 0 || j == DUNGEON_X - 1)
                {
                    dungeon.map[i][j].hardness = (uint8_t)255;
                }
                else
                {
                    dungeon.map[i][j].hardness = (uint8_t)((rand() % 254) + 1);
                }
            }
        }

        for (int i = 0; i < numRooms; i++)
        {

            rooms[i].xSize = (rand() % (ROOM_MAX_X - ROOM_MIN_X + 1)) + ROOM_MIN_X;
            rooms[i].ySize = (rand() % (ROOM_MAX_Y - ROOM_MIN_Y + 1)) + ROOM_MIN_Y;
            rooms[i].xPos = (rand() % (((DUNGEON_X - 1) - rooms[i].xSize) - 1 + 1)) + 1;
            rooms[i].yPos = (rand() % (((DUNGEON_Y - 1) - rooms[i].ySize) - 1 + 1)) + 1;

            dungeon.rooms[i].xPos = rooms[i].xPos;
            dungeon.rooms[i].yPos = rooms[i].yPos;
            dungeon.rooms[i].xSize = rooms[i].xSize;
            dungeon.rooms[i].ySize = rooms[i].ySize;
        }

        int spaceTest = 1;

        for (int room = 0; room < numRooms; room++)
        {
            for (int i = rooms[room].yPos; i < rooms[room].yPos + rooms[room].ySize; i++)
            {
                for (int j = rooms[room].xPos; j < rooms[room].xPos + rooms[room].xSize; j++)
                {
                    if (dungeon.map[i][j].type == ROOM)
                    {
                        spaceTest = 0;
                    }
                    dungeon.map[i][j].type = ROOM;
                    dungeon.map[i][j].hardness = 0;
                }
            }

            for (int i = rooms[room].xPos - 1; i < rooms[room].xPos + rooms[room].xSize + 1; i++)
            {
                if ((dungeon.map[rooms[room].yPos - 1][i].type == ROOM) || (dungeon.map[rooms[room].yPos + rooms[room].ySize][i].type == ROOM))
                {
                    spaceTest = 0;
                    break;
                }
            }

            for (int j = rooms[room].yPos; j < rooms[room].yPos + rooms[room].ySize; j++)
            {
                if ((dungeon.map[j][rooms[room].xPos - 1].type == ROOM) || dungeon.map[j][rooms[room].xPos + rooms[room].xSize].type == ROOM)
                {
                    spaceTest = 0;
                    break;
                }
            }
        }

        if (spaceTest == 1)
        {
            break;
        }
    }

    for (int room = 0; room < numRooms; room++)
    {
        int y = rooms[room].yPos + (rooms[room].ySize / 2);
        int x = rooms[room].xPos + (rooms[room].xSize / 2);

        int nextRoom = room + 1;
        if (nextRoom >= numRooms)
        {
            nextRoom = 0;
        }

        while (1)
        {
            double shortestdistance = sqrt(pow(y - (rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2)), 2) + pow(x - (rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2)), 2));
            double distance = sqrt(pow((y - 1) - (rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2)), 2) + pow(x - (rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2)), 2));

            int yNext = y;
            int xNext = x;
            if (shortestdistance > distance)
            {
                yNext = y - 1;
                xNext = x;
                shortestdistance = distance;
            }
            distance = sqrt(pow((y) - (rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2)), 2) + pow((x - 1) - (rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2)), 2));
            if (shortestdistance > distance)
            {
                yNext = y;
                xNext = x - 1;
                shortestdistance = distance;
            }
            distance = sqrt(pow((y + 1) - (rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2)), 2) + pow((x) - (rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2)), 2));
            if (shortestdistance > distance)
            {
                yNext = y + 1;
                xNext = x;
                shortestdistance = distance;
            }
            distance = sqrt(pow((y) - (rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2)), 2) + pow((x + 1) - (rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2)), 2));
            if (shortestdistance > distance)
            {
                yNext = y;
                xNext = x + 1;
                shortestdistance = distance;
            }

            if (dungeon.map[yNext][xNext].type == ROCK)
            {
                dungeon.map[yNext][xNext].type = CORRIDOR;
                dungeon.map[yNext][xNext].hardness = 0;
            }

            if (yNext == rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2) && xNext == rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2))
            {
                break;
            }

            y = yNext;
            x = xNext;
        }
    }

    int numUp = (rand() % (MAX_UP - MIN_UP + 1)) + MIN_UP;
    int numDown = (rand() % (MAX_DOWN - MIN_DOWN + 1)) + MIN_DOWN;
    for (int i = 0; i < numUp; i++)
    {
        int randroom = rand() % numRooms;
        int y = rooms[randroom].yPos + (rand() % (rooms[randroom].ySize - 1) + 1);
        int x = rooms[randroom].xPos + (rand() % (rooms[randroom].xSize - 1) + 1);
        dungeon.map[y][x].type = UP;
    }
    for (int i = 0; i < numDown; i++)
    {

        while (1)
        {
            int randroom = rand() % numRooms;
            int y = rooms[randroom].yPos + (rand() % (rooms[randroom].ySize - 1) + 1);
            int x = rooms[randroom].xPos + (rand() % (rooms[randroom].xSize - 1) + 1);
            if (dungeon.map[y][x].type == UP)
            {
                continue;
            }
            dungeon.map[y][x].type = DOWN;
            break;
        }
    }

    while (1)
    {
        int pcY = (rand() % (DUNGEON_Y - 1)) + 1;
        int pcX = (rand() % (DUNGEON_X - 1)) + 1;
        if (dungeon.map[pcY][pcX].type == ROOM)
        {
            dungeon.PC.pos.y = pcY;
            dungeon.PC.pos.x = pcX;
            break;
        }
    }
    return dungeon;
}

void printDungeon(dungeon_type *d)
{

    for (int i = 0; i < DUNGEON_Y; i++)
    {

        for (int j = 0; j < DUNGEON_X; j++)
        {
            int out = 0;
            for (int k = 0; k < d->num_monsters; k++)
            {
                if (d->monsters[k].pos.y == i && d->monsters[k].pos.x == j && d->monsters[k].alive)
                {
                    attron(COLOR_PAIR(d->monsters[k].color));
                    mvaddch(i + 1, j, d->monsters[k].to_string);
                    attroff(COLOR_PAIR(d->monsters[k].color));
                    out = 1;
                    break;
                }
            }
            if (out)
            {
                continue;
            }

            if (d->PC.pos.y == i && d->PC.pos.x == j)
            {
                mvaddch(i + 1, j, '@');
            }
            else if (d->map[i][j].type == ROCK)
            {
                mvaddch(i + 1, j, ' ');
            }
            else if (d->map[i][j].type == ROOM)
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
            else if (d->map[i][j].type == CORRIDOR)
            {
                mvaddch(i + 1, j, '#');
            }
            else if (d->map[i][j].type == UP)
            {
                mvaddch(i + 1, j, '<');
            }
            else if (d->map[i][j].type == DOWN)
            {
                mvaddch(i + 1, j, '>');
            }
        }
    }
}
