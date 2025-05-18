#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

#define NUMBER_OF_ITEMS 16
#define MAX_NAME 100
#define MIN_ARMY 1
#define MAX_ARMY 5

#define ERR_UNIT_COUNT "ERR_UNIT_COUNT"
#define ERR_ITEM_COUNT "ERR_ITEM_COUNT"
#define ERR_WRONG_ITEM "ERR_WRONG_ITEM"
#define ERR_SLOTS "ERR_SLOTS"

#define ERR_MISSING_ATTRIBUTE "ERR_MISSING_ATTRIBUTE"
#define ERR_MISSING_VALUE "ERR_MISSING_VALUE"
#define ERR_BAD_VALUE "ERR_BAD_VALUE"

#define ERR_FILE "ERR_FILE"
#define ERR_CMD "ERR_CMD"

#define ERR_SQL "ERR_SQL"

typedef struct item {
    char name[MAX_NAME + 1];
    unsigned int att;
    unsigned int def;
    unsigned int slots;
    unsigned int range;
    unsigned int radius;
} ITEM;

typedef struct unit {
    char name[MAX_NAME + 1];
    const ITEM *item1;
    const ITEM *item2;
    int hp;
} UNIT;

bool check_slots(UNIT unit);

typedef struct {
    struct unit units[5];
    int top;
} ARMY;

void init_army(ARMY *army);

bool is_full(ARMY *army);

bool push(ARMY *army, UNIT *unit);

bool pop_at(ARMY *army, int position);

bool peek_at(const ARMY *army, int position, UNIT *unit);

typedef struct {
    bool army;
    char att_name[MAX_NAME + 1];
    char item[MAX_NAME + 1];
    int item_number;
    char def_name[MAX_NAME + 1];
    int dmg;
} DAMAGE;

typedef struct {
    ITEM *items;
    int count;
} ITEM_LIST;

extern ITEM_LIST item_list;

#endif
