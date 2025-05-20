/**
 * @file main.c
 * @brief Main implementation of the Battle Arena game with ncurses UI
 *
 * This file contains the implementation of a text-based battle arena game
 * using the ncurses library for UI rendering. It allows players to:
 * - Create two armies with up to 5 units each
 * - Equip units with weapons and items
 * - Run battles between the armies with animated combat
 * - View battle results
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For usleep

#include "./include/battle-arena.h"

#define JSON_PATH "./json/items.json"

// Window dimensions
#define GAME_WIDTH 100
#define GAME_HEIGHT 40

// Color pairs
#define COLOR_TITLE 1
#define COLOR_MENU 2
#define COLOR_SELECTED 3
#define COLOR_HP_GOOD 4
#define COLOR_HP_MID 5
#define COLOR_HP_LOW 6
#define COLOR_BATTLE_INFO 7
#define COLOR_ARMY1 8
#define COLOR_ARMY2 9

/**
 * Initializes the ncurses GUI with color pairs and displays a welcome screen
 * Sets up the terminal, color pairs, and cursor settings for the game interface
 */
void init_gui() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Try to resize terminal
    resize_term(GAME_HEIGHT, GAME_WIDTH);

    start_color();

    // Define color pairs
    init_pair(COLOR_TITLE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_MENU, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_SELECTED, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_HP_GOOD, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_HP_MID, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_HP_LOW, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_BATTLE_INFO, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_ARMY1, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_ARMY2, COLOR_RED, COLOR_BLACK);

    // Display a welcome message
    clear();
    box(stdscr, 0, 0);
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(GAME_HEIGHT/2, (GAME_WIDTH-20)/2, "Loading Battle Arena...");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    refresh();
    usleep(500000);  // 0.5 second delay
}

/**
 * Cleans up ncurses environment when exiting the program
 */
void cleanup_gui() {
    endwin();
}

/**
 * Draws a box with ASCII line-drawing characters
 *
 * @param y Y-coordinate of the top-left corner
 * @param x X-coordinate of the top-left corner
 * @param height Height of the box
 * @param width Width of the box
 */
void draw_fancy_box(int y, int x, int height, int width) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width - 1, ACS_URCORNER);
    mvaddch(y + height - 1, x, ACS_LLCORNER);
    mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);

    for (int i = 1; i < width - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + height - 1, x + i, ACS_HLINE);
    }

    for (int i = 1; i < height - 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width - 1, ACS_VLINE);
    }
}

/**
 * Displays available items and allows selection of one item
 *
 * @param is_second Flag indicating if this is the second item selection (allows cancellation)
 * @return Index of selected item in item_list, or -1 if cancelled (only when is_second is true)
 */
int select_item(bool is_second) {
    clear();
    box(stdscr, 0, 0);

    // Draw title box
    draw_fancy_box(1, 4, 3, 50);
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    if (is_second) {
        mvprintw(2, 7, "SELECT SECOND ITEM FOR UNIT");
    } else {
        mvprintw(2, 7, "SELECT FIRST ITEM FOR UNIT");
    }
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    // Draw items box
    draw_fancy_box(5, 4, item_list.count + 4, 50);

    for (int i = 0; i < item_list.count; ++i) {
        ITEM *item = &item_list.items[i];
        mvprintw(7 + i, 7, "%-2d. %-10s ATT:%-3d DEF:%-3d RNG:%-2d RAD:%-2d",
                 i + 1, item->name, item->att, item->def, item->range, item->radius);
    }

    mvprintw(8 + item_list.count, 7, "Enter item number (or 0 to cancel): ");
    refresh();

    char input[8];
    while (1) {
        echo();
        curs_set(1);
        move(8 + item_list.count, 42);
        getnstr(input, sizeof(input) - 1);
        noecho();
        curs_set(0);

        int idx = atoi(input) - 1;
        if (idx >= 0 && idx < item_list.count) {
            return idx;
        }
        if (idx == -1 && is_second) {
            return -1; // Cancel
        }

        attron(COLOR_PAIR(COLOR_HP_LOW));
        mvprintw(9 + item_list.count, 7, "Invalid selection. Try again.");
        attroff(COLOR_PAIR(COLOR_HP_LOW));

        mvprintw(8 + item_list.count, 7, "Enter item number (or 0 to cancel):                ");
        refresh();
    }
}

/**
 * Gets a string input from the user with a specified prompt
 *
 * @param prompt Text to display as the input prompt
 * @param buffer Buffer to store the input string
 * @param maxlen Maximum length of the input string
 */
void get_string_input(const char *prompt, char *buffer, int maxlen) {
    clear();
    box(stdscr, 0, 0);

    // Draw title box
    draw_fancy_box(1, 4, 3, 70);
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 7, "CREATE UNIT");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    // Draw input box
    draw_fancy_box(5, 4, 5, 70);
    mvprintw(7, 7, "%s", prompt);

    echo();
    curs_set(1);
    move(7, strlen(prompt) + 8);
    refresh();
    getnstr(buffer, maxlen - 1);
    noecho();
    curs_set(0);
}

/**
 * Creates an army by prompting the user to input unit details
 * Units can have up to 2 items which must not exceed slot limits
 *
 * @param army Pointer to the ARMY structure to populate
 * @param army_num Army number (1 or 2) for display purposes
 */
void create_army(ARMY *army, int army_num) {
    army->top = -1;
    int unit_count = 0;
    char name[32];

    while (unit_count < 5) {
        clear();
        box(stdscr, 0, 0);

        // Draw title box
        draw_fancy_box(1, 4, 3, 50);
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        mvprintw(2, 7, "CREATE ARMY %d - %d/5 UNITS", army_num, unit_count);
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

        get_string_input("Enter unit name (or leave empty to finish):", name, sizeof(name));
        if (name[0] == '\0') break;

        UNIT unit;
        strncpy(unit.name, name, sizeof(unit.name));

        // Keep asking for valid item combinations until slots check passes
        bool valid_items = false;
        while (!valid_items) {
            int item1_idx = select_item(false);
            unit.item1 = &item_list.items[item1_idx];

            int item2_idx = select_item(true);
            if (item2_idx == -1) {
                unit.item2 = NULL;
            } else {
                unit.item2 = &item_list.items[item2_idx];
            }

            // Check if the selected items exceed slot limit
            if (check_slots(unit)) {
                valid_items = true;
            } else {
                // Display error message
                clear();
                box(stdscr, 0, 0);
                draw_fancy_box(8, 15, 5, 50);
                attron(COLOR_PAIR(COLOR_HP_LOW) | A_BOLD);
                mvprintw(10, 20, "ERROR: SELECTED ITEMS EXCEED 2 SLOTS LIMIT!");
                attroff(COLOR_PAIR(COLOR_HP_LOW) | A_BOLD);
                refresh();
                usleep(1500000); // Show error for 1.5 seconds
            }
        }

        unit.hp = 100;
        push(army, unit);
        unit_count++;

        // Show confirmation
        clear();
        box(stdscr, 0, 0);
        draw_fancy_box(8, 15, 5, 50);
        attron(COLOR_PAIR(COLOR_HP_GOOD));
        mvprintw(10, 25, "UNIT '%s' ADDED TO ARMY %d!", name, army_num);
        attroff(COLOR_PAIR(COLOR_HP_GOOD));
        refresh();
        usleep(800000); // Show confirmation for 0.8 seconds
    }
}

/**
 * Displays the main menu of the game
 * Shows options to create armies, start battle, or exit
 */
void display_menu() {
    clear();
    box(stdscr, 0, 0);

    // Draw title box
    draw_fancy_box(3, 20, 5, 60);
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(5, 43, "BATTLE ARENA");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    // Draw menu box
    draw_fancy_box(12, 30, 14, 40);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(15, 40, "1. Create Army 1");
    mvprintw(18, 40, "2. Create Army 2");
    mvprintw(21, 40, "3. Start Battle");
    mvprintw(24, 40, "4. Exit");
    attroff(COLOR_PAIR(COLOR_MENU));

    // Draw version info
    mvprintw(GAME_HEIGHT-2, 2, "Battle Arena v1.0");

    refresh();
}

/**
 * Draws an HP bar with color coding based on remaining health
 *
 * @param y Y-coordinate to draw the bar
 * @param x X-coordinate to draw the bar
 * @param hp Health points (0-100) to represent
 */
void draw_hp_bar(int y, int x, int hp) {
    int color;
    if (hp > 70) color = COLOR_HP_GOOD;
    else if (hp > 30) color = COLOR_HP_MID;
    else color = COLOR_HP_LOW;

    attron(COLOR_PAIR(color));

    int bar_width = 20;
    int filled = (hp * bar_width) / 100;

    mvprintw(y, x, "[");
    for (int i = 0; i < filled; i++) {
        mvaddch(y, x + 1 + i, '|');
    }
    for (int i = filled; i < bar_width; i++) {
        mvaddch(y, x + 1 + i, ' ');
    }
    mvprintw(y, x + bar_width + 1, "] %d%%", hp);

    attroff(COLOR_PAIR(color));
}

/**
 * Displays the battlefield with both armies and their current status
 *
 * @param army1 Pointer to the first ARMY structure
 * @param army2 Pointer to the second ARMY structure
 * @param round Current battle round number (0 to hide round number)
 */
void display_battlefield(ARMY *army1, ARMY *army2, int round) {
    clear();

    // Draw battlefield border
    box(stdscr, 0, 0);

    // Draw title
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, GAME_WIDTH/2 - 10, "BATTLEFIELD - ROUND %d", round);
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    // Draw army headers
    attron(COLOR_PAIR(COLOR_ARMY1) | A_BOLD);
    mvprintw(4, 5, "ARMY 1");
    attroff(COLOR_PAIR(COLOR_ARMY1) | A_BOLD);

    attron(COLOR_PAIR(COLOR_ARMY2) | A_BOLD);
    mvprintw(4, GAME_WIDTH - 15, "ARMY 2");
    attroff(COLOR_PAIR(COLOR_ARMY2) | A_BOLD);

    // Draw middle line
    for (int i = 5; i < GAME_HEIGHT-5; i++) {
        mvaddch(i, GAME_WIDTH/2, ACS_VLINE);
    }

    // Draw units for army 1
    for (int i = 0; i <= army1->top && i < 5; i++) {
        int y = 6 + i * 5;  // Increased spacing between units

        // Unit box
        draw_fancy_box(y, 3, 4, GAME_WIDTH/2 - 6);

        // Unit info
        attron(COLOR_PAIR(COLOR_ARMY1));
        mvprintw(y+1, 5, "%-15s", army1->units[i].name);
        attroff(COLOR_PAIR(COLOR_ARMY1));

        char items[60] = "";
        if (army1->units[i].item1) {
            strcat(items, army1->units[i].item1->name);
        }
        if (army1->units[i].item2) {
            strcat(items, " & ");
            strcat(items, army1->units[i].item2->name);
        }
        mvprintw(y+1, 21, "Items: %s", items);

        // Draw symbol based on weapon type (using ASCII)
        char symbol[11] = "";
        if (army1->units[i].item1 && army1->units[i].item1->range > 0)
            strcpy(symbol, "->ranged<-");  // Ranged unit
        else
            strcpy(symbol, "XXmeleeXX");  // Melee unit

        mvprintw(y+2, 5, "%s", symbol);

        draw_hp_bar(y+2, GAME_WIDTH/2 - 30, army1->units[i].hp);
    }

    // Draw units for army 2
    for (int i = 0; i <= army2->top && i < 5; i++) {
        int y = 6 + i * 5;  // Increased spacing between units

        // Unit box
        draw_fancy_box(y, GAME_WIDTH/2 + 3, 4, GAME_WIDTH/2 - 6);

        // Unit info
        attron(COLOR_PAIR(COLOR_ARMY2));
        mvprintw(y+1, GAME_WIDTH/2 + 5, "%-15s", army2->units[i].name);
        attroff(COLOR_PAIR(COLOR_ARMY2));

        char items[60] = "";
        if (army2->units[i].item1) {
            strcat(items, army2->units[i].item1->name);
        }
        if (army2->units[i].item2) {
            strcat(items, " & ");
            strcat(items, army2->units[i].item2->name);
        }
        mvprintw(y+1, GAME_WIDTH/2 + 21, "Items: %s", items);

        // Draw symbol based on weapon type (using ASCII)
        char symbol[11] = "";
        if (army2->units[i].item1 && army2->units[i].item1->range > 0)
            strcpy(symbol, "->ranged<-");  // Ranged unit
        else
            strcpy(symbol, "XXmeleeXX");  // Melee unit

        mvprintw(y+2, GAME_WIDTH/2 + 5, "%s", symbol);

        draw_hp_bar(y+2, GAME_WIDTH - 35, army2->units[i].hp);
    }

    // Draw controls
    attron(COLOR_PAIR(COLOR_BATTLE_INFO));
    mvprintw(GAME_HEIGHT-2, GAME_WIDTH/2 - 15, "Press any key for next round...");
    attroff(COLOR_PAIR(COLOR_BATTLE_INFO));

    refresh();
}

/**
 * Animates an attack between armies with moving projectiles
 *
 * @param army1 Pointer to the first ARMY structure
 * @param army2 Pointer to the second ARMY structure
 * @param attacker_side The side that is attacking (1 for army1, 2 for army2)
 */
void animate_attack(ARMY *army1, ARMY *army2, int attacker_side) {
    // Simple animation for attack
    int start_x = attacker_side == 1 ? GAME_WIDTH/2 - 10 : GAME_WIDTH/2 + 10;
    int end_x = attacker_side == 1 ? GAME_WIDTH/2 + 5 : GAME_WIDTH/2 - 5;
    const char* projectiles[3] = {"*", "**", "***"};

    for (int i = 0; i < 5; i++) {
        clear();
        // Always display armies in the same order (army1 first, army2 second)
        display_battlefield(army1, army2, 0); // 0 will not show round number

        // Draw projectile
        int x = attacker_side == 1 ?
                start_x + (i * (end_x - start_x) / 4) :
                end_x + (i * (start_x - end_x) / 4);

        attron(COLOR_PAIR(attacker_side == 1 ? COLOR_ARMY1 : COLOR_ARMY2) | A_BOLD);

        // Choose projectile based on attack type
        const char* projectile;
        if (i < 3) {
            projectile = projectiles[i];
        } else {
            projectile = projectiles[5-i-1];  // Fade out
        }

        mvprintw(GAME_HEIGHT/2, x, "%s", attacker_side == 1 ?
                 "==>" : "<==");
        attroff(COLOR_PAIR(attacker_side == 1 ? COLOR_ARMY1 : COLOR_ARMY2) | A_BOLD);

        refresh();
        usleep(100000); // 0.1 second delay
    }
}

/**
 * Main battle loop that runs the battle between two armies
 * Handles animations, battle rounds, and displays the final result
 *
 * @param army1 Pointer to the first ARMY structure
 * @param army2 Pointer to the second ARMY structure
 */
void battle_loop(ARMY *army1, ARMY *army2) {
    int round = 1;
    int result = -1;

    while (result == -1) {
        display_battlefield(army1, army2, round);
        getch();

        // Animate Army 1 attacking
        animate_attack(army1, army2, 1);

        // Animate Army 2 attacking
        animate_attack(army1, army2, 2);

        result = battle_round(army1, army2);
        round++;
    }

    display_battlefield(army1, army2, round-1);

    draw_fancy_box(GAME_HEIGHT/2-5, GAME_WIDTH/2-25, 10, 50);

    if (result == 0) {
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        mvprintw(GAME_HEIGHT/2-3, GAME_WIDTH/2-6, "IT'S A DRAW!");
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    } else if (result == 1) {
        attron(COLOR_PAIR(COLOR_ARMY1) | A_BOLD);
        mvprintw(GAME_HEIGHT/2-3, GAME_WIDTH/2-11, "ARMY 1 IS VICTORIOUS!");
        attroff(COLOR_PAIR(COLOR_ARMY1) | A_BOLD);
    } else {
        attron(COLOR_PAIR(COLOR_ARMY2) | A_BOLD);
        mvprintw(GAME_HEIGHT/2-3, GAME_WIDTH/2-11, "ARMY 2 IS VICTORIOUS!");
        attroff(COLOR_PAIR(COLOR_ARMY2) | A_BOLD);
    }

    mvprintw(GAME_HEIGHT/2, GAME_WIDTH/2-14, "Press any key to return to menu...");
    refresh();
    getch();
}

/**
 * Main function - entry point of the program
 * Initializes the game, loads items, and runs the main game loop
 *
 * @return 0 on success, non-zero on failure
 */
int main() {
    init_gui();

    FILE *json = fopen(JSON_PATH, "r");
    if (!json) {
        endwin();
        printf("Error: Could not open file %s\n", JSON_PATH);
        error(ERR_FILE);
    }

    load_items(json);
    fclose(json);

    ARMY army1;
    ARMY army2;
    init_army(&army1);
    init_army(&army2);
    bool army1_created = false;
    bool army2_created = false;

    while (1) {
        display_menu();
        int ch = getch();
        if (ch == '1') {
            create_army(&army1, 1);
            army1_created = true;
        } else if (ch == '2') {
            create_army(&army2, 2);
            army2_created = true;
        } else if (ch == '3') {
            if (army1_created && army2_created) {
                battle_loop(&army1, &army2);
                // Reset armies after battle
                init_army(&army1);
                init_army(&army2);
                army1_created = false;
                army2_created = false;
            } else {
                // Error message if armies not created
                clear();
                box(stdscr, 0, 0);
                draw_fancy_box(GAME_HEIGHT/2-3, GAME_WIDTH/2-25, 5, 50);
                attron(COLOR_PAIR(COLOR_HP_LOW) | A_BOLD);
                mvprintw(GAME_HEIGHT/2-1, GAME_WIDTH/2-20, "ERROR: Both armies must be created first!");
                attroff(COLOR_PAIR(COLOR_HP_LOW) | A_BOLD);
                refresh();
                getch();
            }
        } else if (ch == '4') {
            break;
        }
    }

    cleanup_gui();
    return 0;
}

//IMPORTANT: TODO:
//HERE'S HOW TO COMPILE IT gcc main.c src/game.c src/json.c src/logger.c src/structs.c src/utility.c -Iinclude -lncurses -o battle_arena