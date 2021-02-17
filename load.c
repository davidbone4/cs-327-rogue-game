#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h> //TODO

#include "DungeonGenerator.c"
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

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
