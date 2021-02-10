#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rows = 21;
int cols = 80;

typedef struct Cells
{
    char* type[50];
    int* hardness;
} cell;

cell *dungeon[][];

void printCell(struct Cells cell);

int main(int argc, char const *argv[])
{

    typedef struct Cells;

    // cell **dungeon = (cell **)malloc(rows * sizeof(cell *));

    // for (int i = 0; i < rows; i++)
    // {
    //     dungeon[i] = (cell *)malloc(cols * sizeof(cell));
    // }

    struct cell* dungeon[rows][cols];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            dungeon[i][j] = (cell *)malloc(sizeof(cell));
            &(dungeon[i][j])->type = (char*) malloc(sizeof(char)*50);

        }
    }

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

            dungeon[i][j].type = "ROCK";
            printCell(dungeon[i][j]);
        }
        printf("|\n");
    }

    for (int i = 0; i < cols + 2; i++)
    {
        printf("-");
    }

    printf("\n");

    return 0;
}

void initaldungeon(struct cell **cell)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cell[i][j].type =
        }
    }
}

void printCell(struct Cells cell)
{
    if (strcmp(cell.type, "ROCK"))
    {
        printf(" ");
    }
    else if (strcmp(cell.type, "ROOM"))
    {
        printf(".");
    }
    else if (strcmp(cell.type, "CORRIDOR"))
    {
        printf("#");
    }
    else if (strcmp(cell.type, "UP"))
    {
        printf("<");
    }
    else if (strcmp(cell.type, "DOWN"))
    {
        printf(">");
    }
}
