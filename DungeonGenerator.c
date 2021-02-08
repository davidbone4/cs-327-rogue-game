#include <stdio.h>
#include <stdlib.h>

int rows = 21;
int cols = 80;

int main(int argc, char const *argv[])
{

    char **pile = (char **)malloc(rows * sizeof(char *));

    for (int i = 0; i < rows; i++)
    {
        pile[i] = (char *)malloc(cols * sizeof(char));
    }


    for(int i = 0; i < rows; i++){

        for(int j = 0; j < cols; j++){
            pile[i][j] = '#';
            printf("%c", pile[i][j]);
        }
        printf("\n"); }


    return 0;
}