#include <stdio.h>
#include <stdlib.h>

int rows = 21;
int cols = 80;

int main(int argc, char const *argv[])
{

    char **dungeon = (char **)malloc(rows * sizeof(char *));

    for (int i = 0; i < rows; i++)
    {
        dungeon[i] = (char *)malloc(cols * sizeof(char));
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
            dungeon[i][j] = '#';
            printf("%c", dungeon[i][j]);
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
