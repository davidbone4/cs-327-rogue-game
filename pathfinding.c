#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "dungeon.h"
#include "heap.h"

/* This code is based of of Professor Sheaffer's code, but you could probably already tell that...*/

typedef struct corridor_path
{
    heap_node_t *hn;
    uint8_t x, y;
    int32_t cost;
} path_data;

typedef enum dim
{
    dim_x,
    dim_y,
    num_dims
} dim_t;



static int32_t path_cmp(const void *key, const void *with);

static void nontunnel_path_finder(dungeon_type *d);


void print_distance_map(path_data **path);


static int32_t path_cmp(const void *key, const void *with)
{
    return ((path_data *)key)->cost - ((path_data *)with)->cost;
}

static void nontunnel_path_finder(dungeon_type *d)
{
    static path_data path[DUNGEON_Y][DUNGEON_X], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    if (!initialized)
    {
        for (y = 0; y < DUNGEON_Y; y++)
        {
            for (x = 0; x < DUNGEON_X; x++)
            {
                path[y][x].y = y;
                path[y][x].x = x;
            }
        }
        initialized = 1;
    }

    int pcY, pcX;

    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            path[y][x].cost = INT_MAX;
            if (&d->map[y][x].type == PLAYER)
            {
                pcY = y;
                pcX = x;
            }
        }
    }

    path[y][x].cost = 0;

    heap_init(&h, path_cmp, NULL);

    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            if (&d->map[y][x].hardness == 0)
            {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
            }
            else
            {
                path[y][x].hn = NULL;
            }
        }
    }

    while ((p = heap_remove_min(&h)))
    {
        p->hn = NULL;

        for (int i = p->y - 1; i <= p->y + 1; i++)
        {
            for (int j = p->x - 1; j <= p->x + 1; j++)
            {
                if (i == p->y && j == p->x)
                {
                    continue;
                }

                if ((path[i][j].hn) && (path[i][j].cost > p->cost + &d->map[p->y][p->x].hardness))
                {
                    path[i][j].cost = p->cost + &d->map[p->y][p->x].hardness;
                    heap_decrease_key_no_replace(&h, path[i][j].hn);
                }
            }
        }
    }

    print_distance_map(path);
}

void print_distance_map(path_data **path)
{

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            if(path[i][j].cost != INT_MAX){
                printf("%d", path[i][j].cost % 10);
            }else{
                printf(" ");
            }
        }
        printf("\n");
    }
}
