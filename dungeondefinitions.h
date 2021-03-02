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
    uint8_t hardness;
} cell;

typedef struct room8bit
{
    uint8_t xPos;
    uint8_t yPos;
    uint8_t xSize;
    uint8_t ySize;
} room8bit;


typedef struct monster
{
    heap_node_t *hn;
    int speed;
    char to_string;
    char* type;
    uint8_t x,y;
    int nextturn;
    int sequencenumber;
}monster;

typedef struct player
{
    heap_node_t *hn;
    int speed;
    uint8_t x,y;
    int nextturn;
    int sequencenumber;
}player;
typedef struct dungeon
{
    cell map[DUNGEON_Y][DUNGEON_X];
    room8bit rooms[MAX_ROOMS];
    uint16_t num_rooms;
    monster * monsters;
    int num_monsters;
    player PC;
} dungeon_type;




typedef struct corridor_path
{
    heap_node_t *hn;
    uint8_t x, y;
    uint32_t cost;
} path_data;

typedef struct positions
{
    uint8_t x, y;

} position;



typedef enum
{
    ERRATIC,
    TUNNELING,
    TELEPATHY,
    INTELLIGENCE
}behavior;
void init_monsters(dungeon_type *d, int numMonsters);
path_data ** nontunnel_path_finder(dungeon_type *d, int ystart, int xstart);
path_data ** tunnel_path_finder(dungeon_type *d, int ystart, int xstart);
