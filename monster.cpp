#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "dungeondefinitions.h"

extern heap_t h;

const char *hextobinary(int num);

int islineofsight(dungeon_type *d, uint8_t y, uint8_t x, position pc);
static int32_t monster_cmp(const void *key, const void *with);

void movemonsternontunneling(dungeon_type *d, npc *m);
void move_monster_tunneling(dungeon_type *d, npc *m);
void run_game(dungeon_type *d);
int rand_in_range(int min, int max);

void npc::set(const std::string &name,
              const std::string &description,
              const char symbol,
              const uint32_t &color,
              const uint32_t &speed,
              const uint32_t abilities,
              const uint32_t &hitpoints,
              const dice &damage,
              const uint32_t rarity)
{
    this->name = name;
    this->description = description;
    this->to_string = symbol;
    this->color = color;
    this->speed_from_file = speed;
    this->abilities = abilities;
    this->hitpoints = hitpoints;
    this->damage = damage;
    this->rarity = rarity;
}

int rand_in_range(int min, int max)
{
    static bool first = true;
    if (first)
    {
        srand(time(NULL)); //seeding for the first time only!
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

heap_t init_monsters(dungeon_type *d, int numMonsters)
{
    int seed = (unsigned)time(NULL);
    srand(seed);
    // printf("monster seed: %d\n", seed);

    heap_init(&h, monster_cmp, NULL);

    d->PC.hn = heap_insert(&h, &(d->PC));

    npc *monsters = new npc[numMonsters];

    for (int i = 0; i < numMonsters; i++)
    {
        monsters[i] = npc();

        monster_description desc;

        while (1)
        {
            desc = d->monster_descriptions.at(rand_in_range(0, d->monster_descriptions.size() - 1));
            int chance = rand_in_range(1, 100);

            if (chance <= desc.rarity)
            {
                break;
            }
        }

        monsters[i].set(desc.name, desc.description, desc.symbol, desc.color.at(0), desc.speed.roll(), desc.abilities, desc.hitpoints.roll(), desc.damage, desc.rarity);

        monsters[i].speed = monsters[i].speed_from_file;
        monsters[i].type = hextobinary(monsters[i].abilities % 16);

        monsters[i].alive = 1;
        monsters[i].sequencenumber = i + 1;
        monsters[i].nextturn = 0;
        monsters[i].memory.y = 0;
        monsters[i].memory.x = 0;
        monsters[i].isNPC = 1;

        while (1)
        {
            int Y = (rand() % (DUNGEON_Y - 1)) + 1;
            int X = (rand() % (DUNGEON_X - 1)) + 1;
            if ((d->map[Y][X].type == ROOM || d->map[Y][X].type == CORRIDOR) && 25 < (pow(Y - d->PC.pos.y, 2) + pow(X - d->PC.pos.x, 2)))
            {
                monsters[i].pos.y = Y;
                monsters[i].pos.x = X;
                break;
            }
        }

        monsters[i].hn = heap_insert(&h, &(monsters[i]));
    }

    d->monsters = monsters;
    d->num_monsters = numMonsters;

    return h;
}

static int32_t monster_cmp(const void *key, const void *with)
{
    int32_t out = ((character *)key)->nextturn - ((character *)with)->nextturn;
    if (out != 0)
    {
        return out;
    }
    else
    {
        out = ((character *)key)->sequencenumber - ((character *)with)->sequencenumber;
    }

    return out;
}

void movemonsternontunneling(dungeon_type *d, npc *m)
{
    uint8_t nextY = m->pos.y;
    uint8_t nextX = m->pos.x;

    if (m->type[INTELLIGENCE] == '1' && m->type[TELEPATHY] == '1')
    {

        nontunnel_path_finder(d, d->PC.pos.y, d->PC.pos.x);

        for (int y = m->pos.y - 1; y <= m->pos.y + 1; y++)
        {
            for (int x = m->pos.x - 1; x <= m->pos.x + 1; x++)
            {
                if (y == m->pos.y && x == m->pos.x)
                {
                    continue;
                }
                if (d->distance[y][x] < d->distance[m->pos.y][m->pos.x])
                {

                    nextY = y;
                    nextX = x;
                }
            }
        }
    }
    else if (m->type[TELEPATHY] == '1')
    {

        m->memory.y = d->PC.pos.y;
        m->memory.x = d->PC.pos.x;

        double shortestdistance = sqrt(pow(m->pos.y - m->memory.y, 2) + pow(m->pos.x - m->memory.x, 2));
        double distance;

        int shortestY = m->pos.y;
        int shortestX = m->pos.x;

        for (int i = m->pos.y - 1; i < m->pos.y + 2; i++)
        {
            for (int j = m->pos.x - 1; j < m->pos.x + 2; j++)
            {
                if (i == m->pos.y && j == m->pos.x)
                {
                    continue;
                }

                distance = sqrt(pow((i)-m->memory.y, 2) + pow((j)-m->memory.x, 2));
                if (shortestdistance > distance)
                {
                    shortestY = i;
                    shortestX = j;
                    shortestdistance = distance;
                }
            }
        }

        if (d->map[shortestY][shortestX].type != ROCK)
        {
            nextY = shortestY;
            nextX = shortestX;
        }
    }
    else if (m->type[INTELLIGENCE] == '1')
    {
        if (islineofsight(d, m->pos.y, m->pos.x, d->PC.pos))
        {
            m->memory.y = d->PC.pos.y;
            m->memory.x = d->PC.pos.x;
        }

        if (m->memory.y != 0 && m->memory.x != 0)
        {

            nontunnel_path_finder(d, m->memory.y, m->memory.x);

            for (int y = m->pos.y - 1; y <= m->pos.y + 1; y++)
            {
                for (int x = m->pos.x - 1; x <= m->pos.x + 1; x++)
                {
                    if (y == m->pos.y && x == m->pos.x)
                    {
                        continue;
                    }
                    if (d->distance[y][x] < d->distance[m->pos.y][m->pos.x])
                    {

                        nextY = y;
                        nextX = x;
                    }
                }
            }
        }
    }
    else
    {

        if (islineofsight(d, m->pos.y, m->pos.x, d->PC.pos))
        {
            m->memory.y = d->PC.pos.y;
            m->memory.x = d->PC.pos.x;
        }

        if (m->memory.y && m->memory.x)
        {

            double shortestdistance = sqrt(pow(m->pos.y - m->memory.y, 2) + pow(m->pos.x - m->memory.x, 2));
            double distance;

            int shortestY = m->pos.y;
            int shortestX = m->pos.x;

            for (int i = m->pos.y - 1; i < m->pos.y + 2; i++)
            {
                for (int j = m->pos.x - 1; j < m->pos.x + 2; j++)
                {
                    if (i == m->pos.y && j == m->pos.x)
                    {
                        continue;
                    }

                    distance = sqrt(pow((i)-m->memory.y, 2) + pow((j)-m->memory.x, 2));
                    if (shortestdistance > distance)
                    {
                        shortestY = i;
                        shortestX = j;
                        shortestdistance = distance;
                    }
                }
            }

            if (d->map[shortestY][shortestX].type != ROCK)
            {
                nextY = shortestY;
                nextX = shortestX;
            }
        }
    }

    if (m->type[ERRATIC] == '1')
    {
        int chance = rand() % 2;

        if (chance)
        {
            while (1)
            {
                int randY, randX;
                randY = (rand() % 3) + (m->pos.y - 1);
                randX = (rand() % 3) + (m->pos.x - 1);
                if (randY == m->pos.y && randX == m->pos.x)
                {
                    continue;
                }

                if (d->map[randY][randX].type != ROCK)
                {
                    nextY = randY;
                    nextX = randX;
                    break;
                }
            }
        }
    }

    if (m->pos.y != nextY || m->pos.x != nextX)
    {
        if (d->PC.pos.y == nextY && d->PC.pos.x == nextX)
        {
            d->PC.alive = 0;
        }
        for (int i = 0; i < d->num_monsters; i++)
        {
            if (d->monsters[i].pos.y == nextY && d->monsters[i].pos.x == nextX)
            {
                d->monsters[i].alive = 0;
            }
            if (d->monsters[i].pos.y == m->pos.y && d->monsters[i].pos.x == m->pos.x)
            {
                d->monsters[i].pos.y = nextY;
                d->monsters[i].pos.x = nextX;
            }
        }
    }
}

void move_monster_tunneling(dungeon_type *d, npc *m)
{
    uint8_t nextY = m->pos.y;
    uint8_t nextX = m->pos.x;

    if (m->type[INTELLIGENCE] == '1' && m->type[TELEPATHY] == '1')
    {

        tunnel_path_finder(d, d->PC.pos.y, d->PC.pos.x);

        for (int y = m->pos.y - 1; y <= m->pos.y + 1; y++)
        {
            for (int x = m->pos.x - 1; x <= m->pos.x + 1; x++)
            {
                if (y == m->pos.y && x == m->pos.x)
                {
                    continue;
                }
                if (d->pc_tunnel[y][x] < d->pc_tunnel[m->pos.y][m->pos.x])
                {

                    nextY = y;
                    nextX = x;
                }
            }
        }
    }
    else if (m->type[TELEPATHY] == '1')
    {

        m->memory.y = d->PC.pos.y;
        m->memory.x = d->PC.pos.x;

        double shortestdistance = sqrt(pow(m->pos.y - m->memory.y, 2) + pow(m->pos.x - m->memory.x, 2));
        double distance;

        int shortestY = m->pos.y;
        int shortestX = m->pos.x;

        for (int i = m->pos.y - 1; i < m->pos.y + 2; i++)
        {
            for (int j = m->pos.x - 1; j < m->pos.x + 2; j++)
            {
                if (i == m->pos.y && j == m->pos.x)
                {
                    continue;
                }

                distance = sqrt(pow((i)-m->memory.y, 2) + pow((j)-m->memory.x, 2));
                if (shortestdistance > distance)
                {
                    shortestY = i;
                    shortestX = j;
                    shortestdistance = distance;
                }
            }
        }

        if (d->map[shortestY][shortestX].hardness < 255)
        {
            nextY = shortestY;
            nextX = shortestX;
        }
    }
    else if (m->type[INTELLIGENCE] == '1')
    {
        if (islineofsight(d, m->pos.y, m->pos.x, d->PC.pos))
        {
            m->memory.y = d->PC.pos.y;
            m->memory.x = d->PC.pos.x;
        }

        if (m->memory.y != 0 && m->memory.x != 0)
        {

            tunnel_path_finder(d, m->memory.y, m->memory.x);

            for (int y = m->pos.y - 1; y <= m->pos.y + 1; y++)
            {
                for (int x = m->pos.x - 1; x <= m->pos.x + 1; x++)
                {
                    if (y == m->pos.y && x == m->pos.x)
                    {
                        continue;
                    }
                    if (d->pc_tunnel[y][x] < d->pc_tunnel[m->pos.y][m->pos.x])
                    {

                        nextY = y;
                        nextX = x;
                    }
                }
            }
        }
    }
    else
    {

        if (islineofsight(d, m->pos.y, m->pos.x, d->PC.pos))
        {
            m->memory.y = d->PC.pos.y;
            m->memory.x = d->PC.pos.x;
        }

        if (m->memory.y && m->memory.x)
        {

            double shortestdistance = sqrt(pow(m->pos.y - m->memory.y, 2) + pow(m->pos.x - m->memory.x, 2));
            double distance;

            int shortestY = m->pos.y;
            int shortestX = m->pos.x;

            for (int i = m->pos.y - 1; i < m->pos.y + 2; i++)
            {
                for (int j = m->pos.x - 1; j < m->pos.x + 2; j++)
                {
                    if (i == m->pos.y && j == m->pos.x)
                    {
                        continue;
                    }

                    distance = sqrt(pow((i)-m->memory.y, 2) + pow((j)-m->memory.x, 2));
                    if (shortestdistance > distance)
                    {
                        shortestY = i;
                        shortestX = j;
                        shortestdistance = distance;
                    }
                }
            }

            if (d->map[shortestY][shortestX].hardness < 255)
            {
                nextY = shortestY;
                nextX = shortestX;
            }
        }
    }

    if (m->type[ERRATIC] == '1')
    {
        int chance = rand() % 2;

        if (chance)
        {
            while (1)
            {
                int randY, randX;
                randY = (rand() % 3) + (m->pos.y - 1);
                randX = (rand() % 3) + (m->pos.x - 1);
                if (randY == m->pos.y && randX == m->pos.x)
                {
                    continue;
                }

                if (d->map[randY][randX].hardness < 255)
                {
                    nextY = randY;
                    nextX = randX;
                    break;
                }
            }
        }
    }

    if (d->map[nextY][nextX].hardness > 0)
    {
        if (d->map[nextY][nextX].hardness - 85 <= 0)
        {
            d->map[nextY][nextX].hardness = 0;
            d->map[nextY][nextX].type = CORRIDOR;
        }
        else
        {
            d->map[nextY][nextX].hardness -= 85;
            nextY = m->pos.y;
            nextX = m->pos.x;
        }
    }

    if (m->pos.y != nextY || m->pos.x != nextX)
    {
        if (d->PC.pos.y == nextY && d->PC.pos.x == nextX)
        {
            d->PC.alive = 0;
        }
        for (int i = 0; i < d->num_monsters; i++)
        {
            if (d->monsters[i].pos.y == nextY && d->monsters[i].pos.x == nextX)
            {
                d->monsters[i].alive = 0;
            }
            if (d->monsters[i].pos.y == m->pos.y && d->monsters[i].pos.x == m->pos.x)
            {
                d->monsters[i].pos.y = nextY;
                d->monsters[i].pos.x = nextX;
            }
        }
    }
}

const char *hextobinary(int num)
{

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

int islineofsight(dungeon_type *d, uint8_t y, uint8_t x, position pc)
{

    while (1)
    {
        double shortestdistance = sqrt(pow(y - pc.y, 2) + pow(x - pc.x, 2));
        double distance;

        uint8_t yNext = y;
        uint8_t xNext = x;

        for (int i = y - 1; i < y + 2; i++)
        {
            for (int j = x - 1; j < x + 2; j++)
            {
                if (i == y && j == x)
                {
                    continue;
                }

                distance = sqrt(pow((i)-pc.y, 2) + pow((j)-pc.x, 2));
                if (shortestdistance > distance)
                {
                    yNext = i;
                    xNext = j;
                    shortestdistance = distance;
                }
            }
        }

        if (d->map[yNext][xNext].type == ROCK)
        {
            return 0;
        }

        y = yNext;
        x = xNext;

        if (y == pc.y && x == pc.x)
        {
            break;
        }
    }
    return 1;
}
