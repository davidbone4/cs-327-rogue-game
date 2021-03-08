#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "endian.h"

#include "dungeondefinitions.h"

dungeon_type dungeon;

void printDungeon(dungeon_type *d);

int generate();

int load();

int writetodisk();

int main(int argc, char const *argv[])
{
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
    printDungeon(d);

    if (boolsave == 1)
    {
        writetodisk();
    }

    init_monsters(d, nummon);
    run_game(d);

}

int generate()
{
    int seed = (unsigned)time(NULL);
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
    return 0;
}

int load()
{

    char *home = getenv("HOME");
    char *game_dir = ".rlg327";
    char *save_file = "dungeon";

    char *path = malloc((strlen(home) + strlen(game_dir) + strlen(save_file) + 3) * sizeof(char));

    sprintf(path, "%s/%s/%s", home, game_dir, save_file);

    FILE *fr;

    fr = fopen("/Users/davidbone/Library/Mobile Documents/com~apple~CloudDocs/School/spring 2021/saved_dungeons/my_writing/dungeon", "r"); //TODO CHANGEEE THISS
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

    position PC;
    fread(&PC.x, 1, 1, fr);
    fread(&PC.y, 1, 1, fr);

    dungeon.PC.pos.y = PC.y;
    dungeon.PC.pos.x = PC.x;

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            uint8_t givenHardness;
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

    uint16_t numUpStairs;
    fread(&numUpStairs, 2, 1, fr);
    numUpStairs = be16toh(numUpStairs);

    for (int i = 0; i < numUpStairs; i++)
    {
        position upstairs;

        fread(&upstairs.x, 1, 1, fr);
        fread(&upstairs.y, 1, 1, fr);
        dungeon.map[upstairs.y][upstairs.x].type = UP;
    }

    uint16_t numDownStairs;
    fread(&numDownStairs, 2, 1, fr);
    numDownStairs = be16toh(numDownStairs);

    for (int i = 0; i < numDownStairs; i++)
    {
        position downstairs;

        fread(&downstairs.x, 1, 1, fr);
        fread(&downstairs.y, 1, 1, fr);
        dungeon.map[downstairs.y][downstairs.x].type = DOWN;
    }

    fclose(fr);

    return 0;
}

int writetodisk()
{
    uint16_t numUp, numDown;
    numUp = 0;
    numDown = 0;

    position PC;

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            if (dungeon.map[i][j].type == UP)
            {
                numUp++;
            }
            else if (dungeon.map[i][j].type == DOWN)
            {
                numDown++;
            }
        }
    }

    position Upstairs[numUp];
    position Downstairs[numDown];

    int upcounter = 0;
    int downcounter = 0;

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            if (dungeon.map[i][j].type == UP)
            {
                Upstairs[upcounter].x = j;
                Upstairs[upcounter].y = i;
                upcounter++;
            }
            else if (dungeon.map[i][j].type == DOWN)
            {
                Downstairs[downcounter].x = j;
                Downstairs[downcounter].y = i;
                downcounter++;
            }
        }
    }

    char *home = getenv("HOME");
    char *game_dir = ".rlg327";
    char *save_file = "dungeon";

    char *path = malloc((strlen(home) + strlen(game_dir) + strlen(save_file) + 3) * sizeof(char));

    sprintf(path, "%s/%s/%s", home, game_dir, save_file);

    FILE *fw;

    fw = fopen("/Users/davidbone/Library/Mobile Documents/com~apple~CloudDocs/School/spring 2021/saved_dungeons/my_writing/dungeon", "w");

    char *semantic = "RLG327-S2021";
    fwrite(semantic, 1, 12, fw);

    int version = 0;
    version = htobe32(version);
    fwrite(&version, 4, 1, fw);

    int size = 1708 + (dungeon.num_rooms * 4) + (numUp * 2) + (numDown * 2);
    size = htobe32(size);
    fwrite(&size, 4, 1, fw);

    fwrite(&PC.x, 1, 1, fw);
    fwrite(&PC.y, 1, 1, fw);

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            fwrite(&dungeon.map[i][j].hardness, 1, 1, fw);
        }
    }

    uint16_t numrooms_output = dungeon.num_rooms;
    numrooms_output = htobe16(numrooms_output);
    fwrite(&numrooms_output, 2, 1, fw);

    for (int i = 0; i < dungeon.num_rooms; i++)
    {
        fwrite(&dungeon.rooms[i].xPos, 1, 1, fw);
        fwrite(&dungeon.rooms[i].yPos, 1, 1, fw);
        fwrite(&dungeon.rooms[i].xSize, 1, 1, fw);
        fwrite(&dungeon.rooms[i].ySize, 1, 1, fw);
    }

    uint16_t numUp_output = htobe16(numUp);
    fwrite(&numUp_output, 2, 1, fw);

    for (int i = 0; i < numUp; i++)
    {
        fwrite(&Upstairs[i].x, 1, 1, fw);
        fwrite(&Upstairs[i].y, 1, 1, fw);
    }

    uint16_t numDown_output = htobe16(numDown);
    fwrite(&numDown_output, 2, 1, fw);

    for (int i = 0; i < numDown; i++)
    {
        fwrite(&Downstairs[i].x, 1, 1, fw);
        fwrite(&Downstairs[i].y, 1, 1, fw);
    }

    fclose(fw);
    return 0;
}
void printDungeon(dungeon_type *d)
{


    printf("\n");

    for (int i = 0; i < DUNGEON_Y; i++)
    {

        for (int j = 0; j < DUNGEON_X; j++)
        {
            int out = 0;
            for (int k = 0; k < d->num_monsters; k++)
            {
                if (d->monsters[k].pos.y == i && d->monsters[k].pos.x == j && d->monsters[k].alive)
                {
                    printf("%c", d->monsters[k].to_string);
                    out = 1;
                    break;
                }
            }
            if (out)
                continue;
            if (d->PC.pos.y == i && d->PC.pos.x == j)
            {
                printf("@");
            }
            else if (d->map[i][j].type == ROCK)
            {
                printf(" ");
            }
            else if (d->map[i][j].type == ROOM)
            {
                printf(".");
            }
            else if (d->map[i][j].type == CORRIDOR)
            {
                printf("#");
            }
            else if (d->map[i][j].type == UP)
            {
                printf("<");
            }
            else if (d->map[i][j].type == DOWN)
            {
                printf(">");
            }
        }
        printf("\n");
    }


    printf("\n");
}
