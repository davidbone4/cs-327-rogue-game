#include <stdlib.h>
#include <ncurses.h>
#include "heap.h"
#include "descriptions.h"

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
#define SAVE_DIR "rlg327"
#define MONSTER_DESC_FILE "monster_desc.txt"
#define OBJECT_DESC_FILE "object_desc.txt"

typedef enum
{
    ROCK,
    ROOM,
    CORRIDOR,
    UP,
    DOWN
} cellState;

typedef enum
{
    WEAPON,
    OFFHAND,
    RANGED,
    ARMOR,
    HELMET,
    CLOAK,
    GLOVES,
    BOOTS,
    AMULET,
    LIGHT,
    RING_1,
    RING_2,
    NOTYPE = -1
} objects_inventory;
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
    std::string name, description;
    uint32_t color;
    uint32_t abilities;
    int speed_from_file;
    int32_t hitpoints;
    int32_t max_hitpoints;
    dice damage;
    uint32_t rarity;

    npc() : name(), description(), to_string(0), color(0),
            abilities(0), speed_from_file(), hitpoints(), max_hitpoints(),damage(),
            rarity(0)
    {
    }
    void set(const std::string &name,
             const std::string &description,
             const char symbol,
             const uint32_t &color,
             const uint32_t &speed,
             const uint32_t abilities,
             const int32_t &hitpoints,
             const dice &damage,
             const uint32_t rarity);
} npc;

typedef struct object
{
public:
    std::string name, description;
    object_type_t type;
    uint32_t color;
    uint32_t dodge, defence, weight, speed, attribute, value;
    uint32_t hit;
    bool artifact;
    dice damage;
    uint32_t rarity;
    position pos;
    bool picked_up;
    int position_in_dungeon;

    object() : name(), description(), type(objtype_no_type),
               color(0), hit(), damage(dice(0,0,1)),
               dodge(), defence(), weight(),
               speed(0), attribute(), value(),
               artifact(false), rarity(0), pos(), picked_up(),position_in_dungeon()
    {
    }
    void set(const std::string &name,
             const std::string &description,
             const object_type_t type,
             const uint32_t color,
             const uint32_t &hit,
             const dice &damage,
             const uint32_t &dodge,
             const uint32_t &defence,
             const uint32_t &weight,
             const uint32_t &speed,
             const uint32_t &attribute,
             const uint32_t &value,
             const bool artifact,
             const uint32_t rarity,
             const position pos,
             const bool picked_up,
             const int position_in_dungeon);

} object;

typedef class pc : public character
{
public:
    cell map[DUNGEON_Y][DUNGEON_X];
    int32_t hp;
    dice damage;
    object inventory[12];
    object carry[10];
} pc;

typedef class dungeon
{
public:
    cell map[DUNGEON_Y][DUNGEON_X];
    room8bit rooms[MAX_ROOMS];
    uint16_t num_rooms;
    npc *monsters;
    object *objects;
    int num_monsters;
    int num_objects;
    pc PC;
    uint8_t distance[DUNGEON_Y][DUNGEON_X];
    uint8_t pc_tunnel[DUNGEON_Y][DUNGEON_X];
    std::vector<monster_description> monster_descriptions;
    std::vector<object_description> object_descriptions;

} dungeon_type;

typedef struct corridor_path
{
    heap_node_t *hn;
    uint8_t x, y;
    uint32_t cost;
} path_data;

typedef enum
{
    BOSS,
    UNIQUE,
    PICKUP,
    DESTROY,
    PASS,
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
uint32_t parse_descriptions(dungeon_type *d);
uint32_t print_descriptions(dungeon_type *d);
uint32_t destroy_descriptions(dungeon_type *d);
void init_objects(dungeon_type *d);
