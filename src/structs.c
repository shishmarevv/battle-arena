#include <stdio.h>
#include <string.h>

#include "../include/battle-arena.h"

/**
 * Checks if the total number of slots used by a unit's items does not exceed the limit.
 *
 * @param unit The unit whose items are being checked.
 * @return true if the total slots used by the unit's items are less than or equal to 2, false otherwise.
 */
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

/**
 * Finds an item in the global item list by its name.
 *
 * @param name The name of the item to search for.
 * @return A pointer to the ITEM if found, or NULL if not found.
 */
ITEM* find(const char* name) {
    for (int i = 0; i < item_list.count; i++) {
        if (strcasecmp(item_list.items[i].name, name) == 0) return &(item_list.items[i]);
    }
    return NULL;
}

/**
 * Initializes an army by resetting all its units and setting the top index to -1.
 *
 * @param army A pointer to the ARMY structure to initialize.
 */
void init_army(ARMY *army) {
    army->top = -1;

    for (int i = 0; i < MAX_ARMY; i++) {
        army->units[i].hp = 0;
        army->units[i].item1 = NULL;
        army->units[i].item2 = NULL;
        memset(army->units[i].name, 0, sizeof(army->units[i].name));
    }
}

/**
 * Checks if the army is full (i.e., contains the maximum number of units).
 *
 * @param s A pointer to the ARMY structure to check.
 * @return true if the army is full, false otherwise.
 */
bool is_full(ARMY *s) {
    return s->top == 4;
}

/**
 * Adds a unit to the army if there is space available.
 *
 * @param army A pointer to the ARMY structure to add the unit to.
 * @param unit The UNIT to add to the army.
 * @return true if the unit was successfully added, false if the army is full.
 */
bool push(ARMY *army, UNIT unit) {
    if (is_full(army)) {
        return false;
    }
    army->units[++(army->top)] = unit;
    return true;
}

/**
 * Removes a unit from the army at the specified position.
 *
 * @param army A pointer to the ARMY structure to remove the unit from.
 * @param position The index of the unit to remove.
 * @return true if the unit was successfully removed, false if the position is invalid.
 */
bool pop_at(ARMY *army, int position) {
    if (position < 0 || position > army->top) return false;

    for (int i = position; i < army->top; ++i) {
        army->units[i] = army->units[i + 1];
    }
    army->top--;
    return true;
}

/**
 * Retrieves a unit from the army at the specified position without removing it.
 *
 * @param army A pointer to the ARMY structure to retrieve the unit from.
 * @param position The index of the unit to retrieve.
 * @param unit A pointer to a UNIT structure to store the retrieved unit.
 * @return true if the unit was successfully retrieved, false if the position is invalid.
 */
bool peek_at(ARMY *army, int position, UNIT *unit) {
    if (position < 0 || position > army->top) {
        return false;
    }
    *unit = army->units[position];
    return true;
}