#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "./include/battle-arena.h"

#define JSON_PATH "./json/items.json"

void init_gui() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
}

void cleanup_gui() {
    endwin();
}

int select_item(bool is_second) {
    clear();
    if (is_second) {
        mvprintw(2, 5, "Select second item for the unit:");
    } else {
        mvprintw(2, 5, "Select first item for the unit:");
    }
    for (int i = 0; i < item_list.count; ++i) {
        mvprintw(4 + i, 7, "%d. %s", i + 1, item_list.items[i].name);
    }
    refresh();

    char input[8];
    while (1) {
        mvprintw(6 + item_list.count, 7, "Enter item number (or 0 to cancel): ");
        echo();
        curs_set(1);
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
        mvprintw(7 + item_list.count, 7, "Invalid selection. Try again.");
        refresh();
    }
}

void get_string_input(const char *prompt, char *buffer, int maxlen) {
    echo();
    curs_set(1);
    clear();
    mvprintw(2, 5, "%s", prompt);
    move(4, 5);
    getnstr(buffer, maxlen - 1);
    noecho();
    curs_set(0);
}

void create_army(ARMY *army) {
    army->top = -1;
    int unit_count = 0;
    char name[32];
    while (unit_count < 5) {
        get_string_input("Enter unit name (or leave empty to finish):", name, sizeof(name));
        if (name[0] == '\0') break;

        int item_idx = select_item(false);


        UNIT unit;
        strncpy(unit.name, name, sizeof(unit.name));
        unit.item1 = &item_list.items[item_idx];

        item_idx = select_item(true);
        if (item_idx == -1) {
            unit.item2 = NULL;
        } else {
            unit.item2 = &item_list.items[item_idx];
        }

        unit.hp = 100;

        push(army, unit);
        unit_count++;
    }
}

void display_menu() {
    clear();
    mvprintw(2, 5, "BATTLE ARENA");
    mvprintw(4, 5, "1. Create Army 1");
    mvprintw(5, 5, "2. Create Army 2");
    mvprintw(6, 5, "3. Start Battle");
    mvprintw(7, 5, "4. Exit");
    refresh();
}


void display_battlefield(ARMY *army1, ARMY *army2) {
    clear();
    mvprintw(1, 5, "BATTLEFIELD");
    mvprintw(3, 5, "Army 1\t\t\t\tArmy 2");
    for (int i = 0; i < 5; ++i) {
        char unit1[64] = "";
        char unit2[64] = "";
        if (i <= army1->top) {
            snprintf(unit1, sizeof(unit1), "%s (HP: %d)", army1->units[i].name, army1->units[i].hp);
        }
        if (i <= army2->top) {
            snprintf(unit2, sizeof(unit2), "%s (HP: %d)", army2->units[i].name, army2->units[i].hp);
        }
        mvprintw(5 + i, 5, "%-25s | %-25s", unit1, unit2);
    }
    refresh();
}

void battle_loop(ARMY *army1, ARMY *army2) {
    int round = 1;
    int result = -1;
    while (result == -1) {
        display_battlefield(army1, army2);
        mvprintw(12, 5, "Round %d: Press any key for next round...", round++);
        refresh();
        getch();
        result = battle_round(army1, army2);
    }
    display_battlefield(army1, army2);
    if (result == 0)
        mvprintw(14, 5, "Draw!");
    else if (result == 2)
        mvprintw(14, 5, "Army 2 wins!");
    else
        mvprintw(14, 5, "Army 1 wins!");
    mvprintw(16, 5, "Press any key to exit...");
    refresh();
    getch();
}

void display_result(bool winner) {
    clear();
    if (winner)
        mvprintw(5, 10, "NO WINNER");
    else
        mvprintw(5, 10, "WINNER: %d", winner);
    mvprintw(7, 10, "Press any key to exit...");
    refresh();
    getch();
}

int main() {
    init_gui();

    FILE *json = fopen(JSON_PATH, "r");
    if (!json) {
        printf("Error: Could not open file %s\n", JSON_PATH);
        error(ERR_FILE);
    }

    load_items(json);
    fclose(json);

    ARMY army1;
    ARMY army2;


    int choice = 0;
    while (1) {
        display_menu();
        int ch = getch();
        if (ch == '1') {
            init_army(&army1);
            create_army(&army1);
        } else if (ch == '2') {
            init_army(&army2);
            create_army(&army2);
        } else if (ch == '3') {
            battle_loop(&army1, &army2);
            break;
        } else if (ch == '4') {
            break;
        }
    }

    cleanup_gui();
    return 0;
}

//gcc main.c src/game.c src/json.c src/logger.c src/structs.c src/utility.c -Iinclude -lncurses -o battle_arena