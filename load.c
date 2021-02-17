#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h> //TODO


#define _DEFAULT_SOURCE
#define _BSD_SOURCE


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
typedef struct pc
{
    int8_t x, y;

} pc;

int main(int argc, char const *argv[])
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
        printf("File not Found");
        return 0;
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

    pc PC;
    fread(&PC.x, 1, 1, fr);
    fread(&PC.y, 1, 1, fr);

    cell **dungeon = (cell **)malloc(DUNGEON_Y * sizeof(cell *));

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        dungeon[i] = (cell *)malloc(DUNGEON_X * sizeof(cell));
    }

    dungeon[PC.y][PC.x].type = PLAYER;

    printDungeon(dungeon);

    //printf("%s\n", path);

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
