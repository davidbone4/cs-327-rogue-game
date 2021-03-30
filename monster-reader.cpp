#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;

#include "dungeondefinitions.h"

class monster
{
public:
    string name;
    string description;
    string color;
    string speed_from_file;
    string abilities;
    string hp;
    string dam;
    char symbol;
    int rarity;
    monster()
    {
        name = "";
        description = "";
        color = "";
        speed_from_file = "";
        abilities = "";
        hp = "";
        dam = "";
        symbol = '\0';
        rarity = 0;
    }

private:
};

vector<monster> monsters(1, monster());

int main(int argc, char const *argv[])
{
    string dictionary[9] = {"NAME", "COLOR", "SPEED", "ABIL", "HP", "DAM", "SYMB", "RRTY"};

    string name = "";
    string description = "";
    string color = "";
    string speed_from_file = "";
    string abilities = "";
    string hp = "";
    string dam = "";
    char symbol = '\0';
    int rarity = 0;

    string path = getenv("HOME");
    path += "/.rlg327/monster_desc.text";

    ifstream fr("/Users/davidbone/Library/Mobile Documents/com~apple~CloudDocs/School/spring 2021/repos for cs 327/monster_desc.txt"); //TODO CHANGEEE THISS
                                                                                                                                       //TODO CHANGE
    string line;

    getline(fr, line);

    if (line != "RLG327 MONSTER DESCRIPTION 1")
    {
        printf("Wrong File Metadata\n");
        exit(1);
    }
    while (getline(fr, line))
    {

        if (line == "BEGIN MONSTER")
        {
            bool out = false;
            getline(fr, line);
            while (line != "END")
            {

                if (line != "DESC")
                {
                    string key = line.substr(0, line.find(' '));
                    string value = line.substr(line.find(' ') + 1, line.length());

                    int index;
                    for (int i = 0; i < 9; i++)
                    {
                        if (key == dictionary[i])
                        {
                            index = i;
                            break;
                        }
                    }

                    switch (index)
                    {
                    case 0: //"NAME":
                        if (!name.empty())
                        {
                            out = true;
                            break;
                        }
                        name = value;
                        break;
                    case 1: //"COLOR":
                        if (!color.empty())
                        {
                            out = true;
                            break;
                        }
                        color = value;
                        break;
                    case 2: //"SPEED":
                        if (!speed_from_file.empty())
                        {
                            out = true;
                            break;
                        }
                        speed_from_file = value;
                        break;
                    case 3: //"ABIL":
                        if (!abilities.empty())
                        {
                            out = true;
                            break;
                        }
                        abilities = value;
                        break;
                    case 4: //"HP":
                        if (!hp.empty())
                        {
                            out = true;
                            break;
                        }
                        hp = value;
                        break;
                    case 5: //"DAM":
                        if (!dam.empty())
                        {
                            out = true;
                            break;
                        }
                        dam = value;
                        break;
                    case 6: //"SYMB":
                        if (symbol != '\0')
                        {
                            out = true;
                            break;
                        }
                        symbol = value.at(0);
                        break;
                    case 7: //"RRTY":
                        if (rarity != 0)
                        {
                            out = true;
                            break;
                        }
                        rarity = stoi(value);
                        break;
                    }

                    if (out)
                    {
                        break;
                    }
                }
                else
                {
                    if (!description.empty())
                    {
                        out = true;
                        break;
                    }
                    getline(fr, line);
                    while (line != ".")
                    {
                        description += line;
                        description += '\n';
                        getline(fr, line);
                    }
                    description = description.substr(0, description.length() - 1);
                }

                getline(fr, line);
            }

            if (name.empty() || description.empty() || color.empty() || speed_from_file.empty() || abilities.empty() || hp.empty() || dam.empty() || symbol == '\0' || rarity == 0 ||)
            {
                out = true;
            }
            if (!out)
            {
                printf("%s\n", name.c_str());
                printf("%s\n", description.c_str());
                printf("%s\n", color.c_str());
                printf("%s\n", speed_from_file.c_str());
                printf("%s\n", abilities.c_str());
                printf("%s\n", hp.c_str());
                printf("%s\n", dam.c_str());
                printf("%c\n", symbol);
                printf("%d\n", rarity);
                printf("\n");

                monster newmonster;
                newmonster.name = name;
                newmonster.description = description;
                newmonster.color = color;
                newmonster.speed_from_file = speed_from_file;
                newmonster.abilities = abilities;
                newmonster.hp = hp;
                newmonster.dam = dam;
                newmonster.symbol = symbol;
                newmonster.rarity = rarity;

                monsters.push_back(newmonster);
            }

            name = "";
            description = "";
            color = "";
            speed_from_file = "";
            abilities = "";
            hp = "";
            dam = "";
            symbol = '\0';
            rarity = 0;
        }
    }

    fr.close();

    return 0;
}
