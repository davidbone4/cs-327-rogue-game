#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rows = 21;
int cols = 80;

typedef enum
{
    ROCK,
    ROOM,
    CORRIDOR,
    UP,
    DOWN
} cellState;

typedef struct Cells
{
    cellState type;
    int hardness;
} cell;

void printDungeon(cell **dungeon);

int main(int argc, char const *argv[])
{



    cell **dungeon = (cell **)malloc(rows * sizeof(cell *));

    for (int i = 0; i < rows; i++)
    {
        dungeon[i] = (cell *)malloc(cols * sizeof(cell));
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            dungeon[i][j].type = ROCK;
            dungeon[i][j].hardness = 255;
        }
    }




    printDungeon(dungeon);


    return 0;
}

void printDungeon(cell **dungeon)
{
    for (int i = 0; i < cols + 2; i++)
    {
        printf("-");
    }

    printf("\n");

    for (int i = 0; i < rows; i++)
    {
        printf("|");
        for (int j = 0; j < cols; j++)
        {

            if (dungeon[i][j].type == 0)
            {
                printf(" ");
            }
            else if (dungeon[i][j].type== 1)
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
        }
        printf("|\n");
    }

    for (int i = 0; i < cols + 2; i++)
    {
        printf("-");
    }

    printf("\n");
}
