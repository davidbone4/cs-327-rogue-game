#include <stdlib.h>
#include <ncurses.h>
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
    DOWN
} cellState;

typedef enum dim
{
    dim_x,
    dim_y,
    num_dims
} dim_t;

typedef struct positions
{
    uint8_t x, y;

} position;

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

typedef class character
{
public:
    heap_node_t *hn;
    int speed;
    position pos;
    int nextturn;
    int sequencenumber;
    int alive;
    int isNPC;
} character;

typedef class npc : public character
{
public:
    char to_string;
    const char *type;
    position memory;
} npc;

typedef class pc : public character
{
public:
    cell map[DUNGEON_Y][DUNGEON_X];
} pc;

typedef class dungeon
{
public:
    cell map[DUNGEON_Y][DUNGEON_X];
    room8bit rooms[MAX_ROOMS];
    uint16_t num_rooms;
    npc *monsters;
    int num_monsters;
    pc PC;
    uint8_t distance[DUNGEON_Y][DUNGEON_X];
    uint8_t pc_tunnel[DUNGEON_Y][DUNGEON_X];
} dungeon_type;

typedef struct corridor_path
{
    heap_node_t *hn;
    uint8_t x, y;
    uint32_t cost;
} path_data;

typedef enum
{
    ERRATIC,
    TUNNELING,
    TELEPATHY,
    INTELLIGENCE
} behavior;

heap_t init_monsters(dungeon_type *d, int numMonsters);
void nontunnel_path_finder(dungeon_type *d, int ystart, int xstart);
void tunnel_path_finder(dungeon_type *d, int ystart, int xstart);
void movemonsternontunneling(dungeon_type *d, npc *m);
void move_monster_tunneling(dungeon_type *d, npc *m);
void run_game(dungeon_type *d);
void printDungeon(dungeon_type *d);
dungeon_type generate(int seed);
int islineofsight(dungeon_type *d, uint8_t y, uint8_t x, position pc);
