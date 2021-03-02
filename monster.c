#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "dungeondefinitions.h"


char* hextobinary(int num);

monster* init_monsters(dungeon_type *d, int numMonsters)
{

    srand((unsigned)time(NULL));

    monster *monsters = malloc(numMonsters* sizeof(monster));

    for (int i = 0; i < numMonsters; i++)
    {
        monsters[i].speed = (rand() % 16) + 5;
        int type = rand() %  16;
        monsters[i].type = hextobinary(type);

        if(type < 10){
            monsters[i].to_string = (char) type;
        }else{
            char alpha[] = "abcdef";
            monsters[i].to_string = alpha[type - 10];
        }

    }



    return monsters;
}

char* hextobinary(int num){

    switch (num)
    {
    case 0:
        return "0000";
        break;

    case 1:
        return "0001";
        break;

    case 2:
        return "0010";
        break;

    case 3:
        return "0011";
        break;

    case 4:
        return "0100";
        break;

    case 5:
        return "0101";
        break;

    case 6:
        return "0110";
        break;

    case 7:
        return "0111";
        break;
     case 8:
        return "1000";
        break;

    case 9:
        return "1001";
        break;

    case 10:
        return "1010";
        break;

    case 11:
        return "1011";
        break;

     case 12:
        return "1100";
        break;

    case 13:
        return "1101";
        break;

    case 14:
        return "1110";
        break;

    case 15:
        return "1111";
        break;

    }
    return "0000";
}
