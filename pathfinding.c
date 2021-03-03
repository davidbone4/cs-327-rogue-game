#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "dungeondefinitions.h"

static dungeon_type *dungeon;

#define tunnel_movement_cost(x, y)                      \
  ((d->map[y][x].hardness / 85) + 1)
typedef struct path
{
    heap_node_t *hn;
    uint8_t pos[2];
} path_t;

/* This code is based of of Professor Sheaffer's code, but you could probably already tell that...*/

static int cellweight(int hardness);
static int32_t path_cmp(const void *key, const void *with);
static int32_t dist_cmp(const void *key, const void *with);
static int32_t tunnel_cmp(const void *key, const void *with);

void nontunnel_path_finder(dungeon_type *d, int ystart, int xstart);
void tunnel_path_finder(dungeon_type *d, int ystart, int xstart);
static void print_distance_map(dungeon_type *d, int pcY, int pcX);
static void print_distance_map_tunnel(dungeon_type *d, int pcY, int pcX);

static int32_t path_cmp(const void *key, const void *with)
{
    return ((path_data *)key)->cost - ((path_data *)with)->cost;
}

static int32_t dist_cmp(const void *key, const void *with)
{
    return ((int32_t)dungeon->distance[((path_t *)key)->pos[dim_y]]
                                      [((path_t *)key)->pos[dim_x]] -
            (int32_t)dungeon->distance[((path_t *)with)->pos[dim_y]]
                                      [((path_t *)with)->pos[dim_x]]);
}

static int32_t tunnel_cmp(const void *key, const void *with) {
  return ((int32_t) dungeon->pc_tunnel[((path_t *) key)->pos[dim_y]]
                                      [((path_t *) key)->pos[dim_x]] -
          (int32_t) dungeon->pc_tunnel[((path_t *) with)->pos[dim_y]]
                                      [((path_t *) with)->pos[dim_x]]);
}

void nontunnel_path_finder(dungeon_type *d, int ystart, int xstart)
{



    heap_t h;
    uint32_t x, y;
    static path_t p[DUNGEON_Y][DUNGEON_X], *c;
    static uint32_t initialized = 0;

    if (!initialized)
    {
        initialized = 1;
        dungeon = d;
        for (y = 0; y < DUNGEON_Y; y++)
        {
            for (x = 0; x < DUNGEON_X; x++)
            {
                p[y][x].pos[dim_y] = y;
                p[y][x].pos[dim_x] = x;
            }
        }
    }

    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            d->distance[y][x] = 255;
        }
    }
    d->distance[ystart][xstart] = 0;

    heap_init(&h, dist_cmp, NULL);

    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            if (dungeon->map[y][x].hardness == 0)
            {
                p[y][x].hn = heap_insert(&h, &p[y][x]);
            }
        }
    }

    while ((c = heap_remove_min(&h)))
    {
        c->hn = NULL;
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
            (d->distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
            (d->distance[c->pos[dim_y] - 1][c->pos[dim_x]] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] - 1][c->pos[dim_x]] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
            (d->distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
            (d->distance[c->pos[dim_y]][c->pos[dim_x] - 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y]][c->pos[dim_x] - 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
            (d->distance[c->pos[dim_y]][c->pos[dim_x] + 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y]][c->pos[dim_x] + 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
            (d->distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
            (d->distance[c->pos[dim_y] + 1][c->pos[dim_x]] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] + 1][c->pos[dim_x]] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
            (d->distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
             d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
        {
            d->distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
                d->distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
            heap_decrease_key_no_replace(&h,
                                         p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
        }
    }
    heap_delete(&h);


}

void tunnel_path_finder(dungeon_type *d, int ystart, int xstart)
{

  heap_t h;
  uint32_t x, y;
  int size;
  static path_t p[DUNGEON_Y][DUNGEON_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    dungeon = d;
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      d->pc_tunnel[y][x] = 255;
    }
  }
  d->pc_tunnel[ystart][xstart] = 0;

  heap_init(&h, tunnel_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->map[y][x].hardness < 255) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      }
    }
  }

  size = h.size;
  while ((c = heap_remove_min(&h))) {
    if (--size != h.size) {
      exit(1);
    }
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]))) {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
         tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);

  print_distance_map_tunnel(d,d->PC.pos.y,d->PC.pos.x);
}

static int cellweight(int hardness)
{
    if (hardness == 0)
    {
        return 1;
    }
    else
    {
        return 1 + (hardness / 85);
    }
}

static void print_distance_map(dungeon_type *d, int pcY, int pcX)
{

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            if (i == pcY && j == pcX)
            {
                printf("@");
            }
            else if (d->distance[i][j] != 255)
            {
                printf("%d", d->distance[i][j] % 10);
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

static void print_distance_map_tunnel(dungeon_type *d, int pcY, int pcX)
{

    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            if (i == pcY && j == pcX)
            {
                printf("@");
            }
            else if (d->pc_tunnel[i][j] != 255)
            {
                printf("%d", d->pc_tunnel[i][j] % 10);
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}
