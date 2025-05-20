#ifndef BATTLE_ARENA_H
#define BATTLE_ARENA_H

#include <stdbool.h>
#include <stdio.h>
#include <ncurses.h>

#define MENU_HEIGHT 15
#define MENU_WIDTH 40
#define ARMY_WIN_HEIGHT 15
#define ARMY_WIN_WIDTH 60
#define BATTLE_HEIGHT 20
#define BATTLE_WIDTH 80

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

#define ERR_MEMORY "ERR_MEMORY"

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

bool push(ARMY *army, UNIT unit);

bool pop_at(ARMY *army, int position);

bool peek_at(ARMY *army, int position, UNIT *unit);


typedef struct {
    ITEM *items;
    int count;
} ITEM_LIST;

extern ITEM_LIST item_list;

void load_items(FILE *json);

ITEM* find(const char* name);

int max(int a, int b);

void info (const char *message);

void error(const char *message);

void warning(const char *message);


typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
} LogLevel;

extern LogLevel CURRENT_LOG_LEVEL;

int  log_init(const char *filename, LogLevel level);

void log_close(void);

void log_message(LogLevel lvl, const char *message, ...);

#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, "DEBUG: " fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message(LOG_LEVEL_INFO,  "INFO:  " fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message(LOG_LEVEL_WARN,  "WARN:  " fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, "ERROR: " fmt, ##__VA_ARGS__)


typedef enum {
    MENU,
    CREATE_ARMY_1,
    CREATE_ARMY_2,
    BATTLE,
    RESULT
} UIState;

typedef struct {
    WINDOW *main_win;
    WINDOW *status_win;
    UIState current_state;
    int max_rounds;
    int current_round;
    ARMY army1;
    ARMY army2;
} GameUI;


void apply_damage(ARMY *target_army, int position, int damage);
void attack(ARMY *attacking_army, ARMY *defending_army);
void shift_positions(ARMY *army1, ARMY *army2);
int battle_round(ARMY *army1, ARMY *army2);
#endif
