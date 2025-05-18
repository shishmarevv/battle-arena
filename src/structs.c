#include <stdio.h>
#include <string.h>

#include "../include/structs.h"

bool check_slots(const UNIT unit) {
    int total_slots = 0;

    if (unit.item1) {
        total_slots += unit.item1->slots;
    }

    if (unit.item2) {
        total_slots += unit.item2->slots;
    }

    return total_slots <= 2;
}

void init_army(ARMY *army) {
    army -> top = -1;

    for (int i = 0; i < MAX_ARMY; i++) {
        army -> units[i].hp = 0;
        army -> units[i].item1 = NULL;
        army -> units[i].item2 = NULL;
        memset(army -> units[i].name, 0, sizeof(army -> units[i].name));
    }
}

bool is_full(const ARMY *s) {
    return s->top == 4;
}

bool push(ARMY *army, const UNIT unit) {
    if (is_full(army)) {
        return false;
    }
    army->units[++(army->top)] = unit;
    return true;
}


bool pop_at(ARMY *army, const int position) {
    if (position < 0 || position > army->top) return false;

    for (int i = position; i < army->top; ++i) {
        army->units[i] = army->units[i + 1];
    }
    army->top--;
    return true;
}


bool peek_at(const ARMY *army, const int position, UNIT *unit) {
    if (position < 0 || position > army->top) {
        return false;
    }
    *unit = army->units[position];
    return true;
}