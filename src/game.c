#include "../include/battle-arena.h"
#include <stdlib.h>

void apply_damage(ARMY *target_army, int position, int damage) {
    if (position < 0 || position > target_army->top) return;

    target_army->units[position].hp -= damage;
    if (target_army->units[position].hp <= 0) {
        pop_at(target_army, position);
    }
}

void attack(ARMY *attacking_army, ARMY *defending_army) {
    for (int i = 0; i <= attacking_army->top; i++) {
        UNIT attacker = attacking_army->units[i];

        for (int j = 0; j < 2; j++) {
            const ITEM *item = (j == 0) ? attacker.item1 : attacker.item2;
            if (!item) continue;

            int range = item->range;
            int radius = item->radius;
            int att = item->att;

            for (int k = 0; k <= defending_army->top; k++) {
                int position_diff = abs((4 - i) - (5 + k));
                if (position_diff > range) continue;

                int def = 0;
                if (defending_army->units[k].item1) def += defending_army->units[k].item1->def;
                if (defending_army->units[k].item2) def += defending_army->units[k].item2->def;

                int damage = max(1, att - def);
                apply_damage(defending_army, k, damage);

                if (radius == 0) break; // Single target
            }
        }
    }
}

void shift_positions(ARMY *army1, ARMY *army2) {
    if (army1->top >= 0) {
        for (int i = 0; i <= army1->top; i++) {
            if (i < 4) army1->units[i] = army1->units[i + 1];
        }
    }

    if (army2->top >= 0) {
        for (int i = army2->top; i > 0; i--) {
            army2->units[i] = army2->units[i - 1];
        }
    }
}

int battle_round(ARMY *army1, ARMY *army2) {
    attack(army1, army2);
    attack(army2, army1);

    shift_positions(army1, army2);

    if (army1->top < 0 && army2->top < 0) return 0; // Draw
    if (army1->top < 0) return 2; // Army 2 wins
    if (army2->top < 0) return 1; // Army 1 wins

    return -1; // Continue battle
}