#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/battle-arena.h"

#include <string.h>

/**
 * Storage for all items in the game.
 * This is a static array with a fixed maximum size.
 */
static ITEM items_storage[NUMBER_OF_ITEMS];

/**
 * Global list of all available items.
 * Contains a pointer to the items array and a count of loaded items.
 */
ITEM_LIST item_list = {items_storage, 0};

/**
 * Skips whitespace and newline characters in a file stream.
 * Advances the file pointer until a non-whitespace character is found.
 *
 * @param file The file stream to read from
 * @return The first non-whitespace character found, or EOF if end of file is reached
 */
int skip(FILE *file) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (!isspace(c) && c != '\n') {
            return c;
        }
    }
    return EOF;
}

/**
 * Loads item definitions from a JSON file into the global item list.
 * Parses a JSON array of item objects, each containing name, att, def, slots, range, and radius attributes.
 *
 * The function expects a JSON structure like:
 * [
 *   {
 *     "name": "ItemName",
 *     "att": AttackValue,
 *     "def": DefenseValue,
 *     "slots": SlotsRequired,
 *     "range": AttackRange,
 *     "radius": EffectRadius
 *   },
 *   ...
 * ]
 *
 * @param json The file stream containing JSON data to parse
 */
void load_items(FILE *json) {
    item_list.count = 0;

    int c;
    while ((c = fgetc(json)) != EOF && c != '[') {}

    while ((c = fgetc(json)) != EOF && c != ']') {
        c = skip(json);
        if (c == '{') {
            ITEM item = {0};
            bool attributes[6] = {false};

            while ((c = fgetc(json)) != EOF && c != '}') {
                c = skip(json);
                if (c == '}') {
                    break;
                }
                if (c == '"') {
                    char key[MAX_NAME];
                    int i = 0;

                    while ((c = fgetc(json)) != EOF && c != '"' && i < MAX_NAME) {
                        key[i++] = (char) c;
                    }
                    key[i] = '\0';

                    c = skip(json);
                    if (c == ':') {
                        if (strcmp(key, "name") == 0) {
                            c = skip(json);
                            if (c == '"') {
                                i = 0;
                                while ((c = fgetc(json)) != EOF && c != '"' && i < MAX_NAME) {
                                    item.name[i++] = (char) c;
                                }
                                item.name[i] = '\0';
                                attributes[0] = true;
                            } else if (c == ',' || c == '}') {
                                error(ERR_MISSING_VALUE);
                            } else {
                                error(ERR_BAD_VALUE);
                            }
                        } else if (strcmp(key, "att") == 0) {
                            c = skip(json);
                            ungetc(c, json);
                            if (fscanf(json, "%d", &item.att) == 1) {
                                attributes[1] = true;
                            } else {
                                if (c == ',' || c == '}') {
                                    error(ERR_MISSING_VALUE);
                                } else {
                                    error(ERR_BAD_VALUE);
                                }
                            }
                        } else if (strcmp(key, "def") == 0) {
                            c = skip(json);
                            ungetc(c, json);
                            if (fscanf(json, "%d", &item.def) == 1) {
                                attributes[2] = true;
                            } else {
                                if (c == ',' || c == '}') {
                                    error(ERR_MISSING_VALUE);
                                } else {
                                    error(ERR_BAD_VALUE);
                                }
                            }
                        } else if (strcmp(key, "slots") == 0) {
                            c = skip(json);
                            ungetc(c, json);
                            if (fscanf(json, "%d", &item.slots) == 1) {
                                attributes[3] = true;
                            } else {
                                if (c == ',' || c == '}') {
                                    error(ERR_MISSING_VALUE);
                                } else {
                                    error(ERR_BAD_VALUE);
                                }
                            }
                        } else if (strcmp(key, "range") == 0) {
                            c = skip(json);
                            ungetc(c, json);
                            if (fscanf(json, "%d", &item.range) == 1) {
                                attributes[4] = true;
                            } else {
                                if (c == ',' || c == '}') {
                                    error(ERR_MISSING_VALUE);
                                } else {
                                    error(ERR_BAD_VALUE);
                                }
                            }
                        } else if (strcmp(key, "radius") == 0) {
                            c = skip(json);
                            ungetc(c, json);
                            if (fscanf(json, "%d", &item.radius) == 1) {
                                attributes[5] = true;
                            } else {
                                if (c == ',' || c == '}') {
                                    error(ERR_MISSING_VALUE);
                                } else {
                                    error(ERR_BAD_VALUE);
                                }
                            }
                        }
                    }
                }
            }
            if (attributes[0] && attributes[1] && attributes[2] && attributes[3] && attributes[4] && attributes[5]) {
                if (item_list.count >= NUMBER_OF_ITEMS) {
                    error("ERR_TOO_MANY_ITEMS");
                }
                item_list.items[item_list.count] = item;
                item_list.count++;
            } else {
                error(ERR_MISSING_ATTRIBUTE);
            }
        }
    }
}