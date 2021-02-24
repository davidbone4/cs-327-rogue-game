#include <stdlib.h>
#include "heap.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define MIN_ROOMS 6
#define MAX_ROOMS 10
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 12
#define ROOM_MAX_Y 12
#define MIN_UP 1
#define MAX_UP 3
#define MIN_DOWN 1
#define MAX_DOWN 3

typedef enum
{
    ROCK,
    ROOM,
    CORRIDOR,
    UP,
    DOWN,
    PLAYER
} cellState;

typedef struct Cells
{
    cellState type;
    int8_t hardness;
} cell;

typedef struct room8bit
{
    int8_t xPos;
    int8_t yPos;
    int8_t xSize;
    int8_t ySize;
} room8bit;

typedef struct dungeon
{
    cell map[DUNGEON_Y][DUNGEON_X];
    room8bit rooms[MAX_ROOMS];
    int16_t num_rooms;
} dungeon_type;



typedef struct corridor_path
{
    heap_node_t *hn;
    uint8_t x, y;
    int32_t cost;
} path_data;

typedef struct positions
{
    int8_t x, y;

} position;


void nontunnel_path_finder(dungeon_type *d);
