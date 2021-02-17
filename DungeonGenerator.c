#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>




#define DUNGEON_X              80
#define DUNGEON_Y              21
#define MIN_ROOMS              6
#define MAX_ROOMS              10
#define ROOM_MIN_X             4
#define ROOM_MIN_Y             3
#define ROOM_MAX_X             12
#define ROOM_MAX_Y             12

typedef enum
{
    ROCK,
    ROOM,
    CORRIDOR,
    UP,
    DOWN,
    PLAYER
} cellState;

typedef struct Cells
{
    cellState type;
    int hardness;
} cell;

typedef struct room
{
    int xPos;
    int yPos;
    int xSize;
    int ySize;
} room;

void printDungeon(cell **dungeon);

int mainbae(int argc, char const *argv[])
{

    cell **dungeon = (cell **)malloc(DUNGEON_Y * sizeof(cell *));

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        dungeon[i] = (cell *)malloc(DUNGEON_X * sizeof(cell));
    }

    srand((unsigned)time(NULL));

    int numRooms = (rand() % (MAX_ROOMS - MIN_ROOMS + 1)) + MIN_ROOMS;

    struct room rooms[numRooms];

    while (1)
    {

        for (int i = 0; i < DUNGEON_Y; i++)
        {
            for (int j = 0; j < DUNGEON_X; j++)
            {
                dungeon[i][j].type = ROCK;
                dungeon[i][j].hardness = 255;
            }
        }

        for (int i = 0; i < numRooms; i++)
        {

            rooms[i].xSize = (rand() % (ROOM_MAX_X - ROOM_MIN_X + 1)) + ROOM_MIN_X;
            rooms[i].ySize = (rand() % (ROOM_MAX_Y - ROOM_MIN_Y + 1)) + ROOM_MIN_Y;
            rooms[i].xPos = (rand() % (((DUNGEON_X-1) - rooms[i].xSize) - 1 + 1)) + 1;
            rooms[i].yPos = (rand() % (((DUNGEON_Y -1) - rooms[i].ySize) - 1 + 1)) + 1;
        }

        int spaceTest = 1;

        for (int room = 0; room < numRooms; room++)
        {
            for (int i = rooms[room].yPos; i < rooms[room].yPos + rooms[room].ySize; i++)
            {
                for (int j = rooms[room].xPos; j < rooms[room].xPos + rooms[room].xSize; j++)
                {
                    if (dungeon[i][j].type == ROOM)
                    {
                        spaceTest = 0;
                    }
                    dungeon[i][j].type = ROOM;
                    dungeon[i][j].hardness = -1;
                }
            }

            for (int i = rooms[room].xPos - 1; i < rooms[room].xPos + rooms[room].xSize + 1; i++)
            {
                if ((dungeon[rooms[room].yPos - 1][i].type == ROOM) || (dungeon[rooms[room].yPos + rooms[room].ySize][i].type == ROOM))
                {
                    spaceTest = 0;
                    break;
                }
            }

            for (int j = rooms[room].yPos; j < rooms[room].yPos + rooms[room].ySize; j++)
            {
                if ((dungeon[j][rooms[room].xPos - 1].type == ROOM) || dungeon[j][rooms[room].xPos + rooms[room].xSize].type == ROOM)
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

            if (dungeon[yNext][xNext].type == ROCK)
            {
                dungeon[yNext][xNext].type = CORRIDOR;
            }

            if (yNext == rooms[nextRoom].yPos + (rooms[nextRoom].ySize / 2) && xNext == rooms[nextRoom].xPos + (rooms[nextRoom].xSize / 2))
            {
                break;
            }

            y = yNext;
            x = xNext;
        }
    }

    int randroom = rand() % numRooms;
    int y = rooms[randroom].yPos + (rand() % (rooms[randroom].ySize - 1) + 1);
    int x = rooms[randroom].xPos + (rand() % (rooms[randroom].xSize - 1) + 1);
    dungeon[y][x].type = UP;

    while (1)
    {
        randroom = rand() % numRooms;
        int y = rooms[randroom].yPos + (rand() % (rooms[randroom].ySize - 1) + 1);
        int x = rooms[randroom].xPos + (rand() % (rooms[randroom].xSize - 1) + 1);
        if (dungeon[y][x].type == UP)
        {
            continue;
        }
        dungeon[y][x].type = DOWN;
        break;
    }

    printDungeon(dungeon);

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        free(dungeon[i]);
    }

    free(dungeon);

    return 0;
}

void printDungeon(cell **dungeon)
{
    for (int i = 0; i < DUNGEON_X + 2; i++)
    {
        printf("-");
    }

    printf("\n");

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        printf("|");
        for (int j = 0; j < DUNGEON_X; j++)
        {

            if (dungeon[i][j].type == 0)
            {
                printf(" ");
            }
            else if (dungeon[i][j].type == 1)
            {
                printf(".");
            }
            else if (dungeon[i][j].type == 2)
            {
                printf("#");
            }
            else if (dungeon[i][j].type == 3)
            {
                printf("<");
            }
            else if (dungeon[i][j].type == 4)
            {
                printf(">");
            }
            else if (dungeon[i][j].type == 5)
            {
                printf("@");
            }
        }
        printf("|\n");
    }

    for (int i = 0; i < DUNGEON_X + 2; i++)
    {
        printf("-");
    }

    printf("\n");
}
