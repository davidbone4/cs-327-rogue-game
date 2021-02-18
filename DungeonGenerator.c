#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "endian.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define MIN_ROOMS 6
#define MAX_ROOMS 10
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 12
#define ROOM_MAX_Y 12
#define MIN_UP 1
#define MAX_UP 3
#define MIN_DOWN 1
#define MAX_DOWN 3

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

typedef struct room8bit
{
    int8_t xPos;
    int8_t yPos;
    int8_t xSize;
    int8_t ySize;
} room8bit;

typedef struct dungeon
{
    cell map[DUNGEON_Y][DUNGEON_X];
    room8bit rooms[MAX_ROOMS];
    int16_t num_rooms;
} dungeon_type;
typedef struct positions
{
    int8_t x, y;

} position;

void printDungeon(dungeon_type dungeon);

int generate(dungeon_type dungeon);

int load(dungeon_type dungeon);

int main(int argc, char const *argv[])
{

    // cell **dungeon = (cell **)malloc(DUNGEON_Y * sizeof(cell *));

    // for (int i = 0; i < DUNGEON_Y; i++)
    // {
    //     dungeon[i] = (cell *)malloc(DUNGEON_X * sizeof(cell));
    // }

    dungeon_type dungeon;

   // load(dungeon);


    generate(dungeon);
}

int generate(dungeon_type dungeon)
{

    srand((unsigned)time(NULL));

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
                dungeon.map[i][j].hardness = 255;
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

    for(int i = 0; i < DUNGEON_Y; i++){
        int out = 0;
        for(int j = 0; j<DUNGEON_X; j++){
            if(dungeon.map[i][j].type == ROOM){
                dungeon.map[i][j].type = PLAYER;
                out = 1;
                break;
            }
        }
        if(out == 1){
            break;
        }
    }

    printDungeon(dungeon);

    return 0;
}

int load(dungeon_type dungeon)
{

    char *home = getenv("HOME");
    char *game_dir = ".rlg327";
    char *save_file = "dungeon";

    char *path = malloc((strlen(home) + strlen(game_dir) + strlen(save_file) + 3) * sizeof(char));

    sprintf(path, "%s/%s/%s", home, game_dir, save_file);

    FILE *fr;

    fr = fopen("/Users/davidbone/Library/Mobile Documents/com~apple~CloudDocs/School/spring 2021/saved_dungeons/00.rlg327", "r"); //TODO CHANGEEE THISS
                                                                                                                                                 //TODO CHANGE

    if (fr == NULL)
    {
        printf("File not Found\n");
        return -1;
    }

    char semantic[13];
    semantic[12] = '\0';
    fread(semantic, 1, 12, fr);

    int version;
    fread(&version, 4, 1, fr);
    version = be32toh(version);

    int size;
    fread(&size, 4, 1, fr);
    size = be32toh(size);
    printf("%d",size);

    position PC;
    fread(&PC.x, 1, 1, fr);
    fread(&PC.y, 1, 1, fr);

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            int8_t givenHardness;
            fread(&givenHardness, 1, 1, fr);
            dungeon.map[i][j].hardness = givenHardness;
            if (givenHardness == 0)
            {
                dungeon.map[i][j].type = CORRIDOR;
            }
            else
            {
                dungeon.map[i][j].type = ROCK;
            }
        }
    }

    int numRooms;
    fread(&numRooms, 2, 1, fr);
    numRooms = be16toh(numRooms);
    dungeon.num_rooms = numRooms;

    room8bit rooms[numRooms];

    for (int i = 0; i < numRooms; i++)
    {
        fread(&rooms[i].xPos, 1, 1, fr);
        fread(&rooms[i].yPos, 1, 1, fr);
        fread(&rooms[i].xSize, 1, 1, fr);
        fread(&rooms[i].ySize, 1, 1, fr);

        dungeon.rooms[i].xPos = rooms[i].xPos;
        dungeon.rooms[i].yPos = rooms[i].yPos;
        dungeon.rooms[i].xSize = rooms[i].xSize;
        dungeon.rooms[i].ySize = rooms[i].ySize;
    }

    for (int idx = 0; idx < numRooms; idx++)
    {

        for (int i = rooms[idx].yPos; i < rooms[idx].yPos + rooms[idx].ySize; i++)
        {
            for (int j = rooms[idx].xPos; j < rooms[idx].xPos + rooms[idx].xSize; j++)
            {
                dungeon.map[i][j].type = ROOM;
            }
        }
    }

    int16_t numUpStairs;
    fread(&numUpStairs, 2, 1, fr);
    numUpStairs = be16toh(numUpStairs);

    for (int i = 0; i < numUpStairs; i++)
    {
        position upstairs;

        fread(&upstairs.x, 1, 1, fr);
        fread(&upstairs.y, 1, 1, fr);
        dungeon.map[upstairs.y][upstairs.x].type = UP;
    }

    int16_t numDownStairs;
    fread(&numDownStairs, 2, 1, fr);
    numDownStairs = be16toh(numDownStairs);

    for (int i = 0; i < numDownStairs; i++)
    {
        position downstairs;

        fread(&downstairs.x, 1, 1, fr);
        fread(&downstairs.y, 1, 1, fr);
        dungeon.map[downstairs.y][downstairs.x].type = DOWN;
    }

    dungeon.map[PC.y][PC.x].type = PLAYER;

    printDungeon(dungeon);

    fclose(fr);

    return 0;
}

int write(dungeon_type dungeon)
{
    int16_t numUp;
    return 0;
}
void printDungeon(dungeon_type dungeon)
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

            if (dungeon.map[i][j].type == 0)
            {
                printf(" ");
            }
            else if (dungeon.map[i][j].type == 1)
            {
                printf(".");
            }
            else if (dungeon.map[i][j].type == 2)
            {
                printf("#");
            }
            else if (dungeon.map[i][j].type == 3)
            {
                printf("<");
            }
            else if (dungeon.map[i][j].type == 4)
            {
                printf(">");
            }
            else if (dungeon.map[i][j].type == 5)
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
