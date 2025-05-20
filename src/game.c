#include <string.h>

#include "../include/battle-arena.h"

/**
 * Removes units with zero or negative HP from the army.
 * Iterates through the army from top to bottom to safely remove dead units.
 *
 * @param army Pointer to the ARMY structure to check and update
 */
void check_hp(ARMY *army) {
    for (int i = army -> top; i >= 0; i--) {
        if (army ->units[i].hp <= 0) {
            pop_at(army, i);
        }
    }
}

/**
 * Executes attack actions from one army against another.
 * For each unit in the attacking army:
 *   - For each item the unit has:
 *     - If the unit's position is within item's range:
 *       - Attack all defending units within the item's radius
 *       - Damage is calculated as max(attacker's attack - defender's defense, 1)
 *
 * @param attacking_army Pointer to the ARMY structure that is attacking
 * @param defending_army Pointer to the ARMY structure that is defending
 */
void attack(ARMY *attacking_army, ARMY *defending_army) {
    for (int i = 0; i <= attacking_army -> top; i++) {
            UNIT attacker;
            if (peek_at(attacking_army, i, &attacker)) {
                if (attacker.item1) {
                    if (attacker.item1->range >= i) {
                        for (int j = 0; j <= attacker.item1->radius; j++) {
                            UNIT defender;
                            if (peek_at(defending_army, j, &defender)) {
                                int de = 0;
                                if (defender.item1) {
                                    de += defender.item1->def;
                                }
                                if (defender.item2) {
                                    de += defender.item2->def;
                                }
                                const int d = max(attacker.item1 -> att - de, 1);
                                defending_army -> units[j].hp -= d;
                            }
                        }
                    }
                }
                if (attacker.item2) {
                    if (attacker.item2 -> range >= i) {
                        for (int j = 0; j <= attacker.item2->radius; j++) {
                            UNIT defender;
                            if (peek_at(defending_army, j, &defender)) {
                                int de = 0;
                                if (defender.item1) {
                                    de += defender.item1->def;
                                }
                                if (defender.item2) {
                                    de += defender.item2->def;
                                }
                                const int d = max(attacker.item2 -> att - de, 1);
                                defending_army -> units[j].hp -= d;
                            }
                        }
                    }
                }
            }
        }
}

/**
 * Executes a single round of battle between two armies.
 * The process consists of:
 *   1. Army 1 attacks Army 2
 *   2. Army 2 attacks Army 1
 *   3. Dead units are removed from both armies
 *   4. Victory/defeat conditions are checked
 *
 * @param army1 Pointer to the first ARMY structure
 * @param army2 Pointer to the second ARMY structure
 * @return int Result code:
 *         -1: Battle continues (no winner yet)
 *          0: Draw (both armies defeated)
 *          1: Army 1 wins
 *          2: Army 2 wins
 */
int battle_round(ARMY *army1, ARMY *army2) {
    attack(army1, army2);
    attack(army2, army1);

    check_hp(army1);
    check_hp(army2);


    if (army1->top < 0 && army2->top < 0) return 0; // Draw
    if (army1->top < 0) return 2; // Army 2 wins
    if (army2->top < 0) return 1; // Army 1 wins

    return -1; // Continue battle
}