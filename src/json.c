#include <ctype.h>
#include <stdio.h>

#include "../include/json.h"

#include <string.h>

#include "../include/utility.h"
#include "../include/structs.h"
#include "../include/sql.h"

int skip(FILE *file) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (!isspace(c) && c != '\n') {
            return c;
        }
    }
    return EOF;
}

void load_items(FILE *json) {
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
                insert_item(item);
            } else {
                error(ERR_MISSING_ATTRIBUTE);
            }
        }
    }
}