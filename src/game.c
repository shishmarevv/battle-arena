#include <string.h>

#include "../include/battle-arena.h"


void check_hp(ARMY *army) {
    for (int i = army -> top; i >= 0; i--) {
        if (army ->units[i].hp <= 0) {
            pop_at(army, i);
        }
    }
}

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