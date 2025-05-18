#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/structs.h"


void insert_item(const ITEM item) {
    sqlite3 *db;
    sqlite3_stmt *stmt;

    log_init("../log.txt", LOG_LEVEL_DEBUG);
    int rc = sqlite3_open("../data/data.db", &db);
    if (rc != SQLITE_OK) LOG_WARN("%s rc - %d", ERR_SQL, rc);

    const char *sql = "INSERT INTO items(name, attack, defense, slots, range, radius) VALUES(?, ?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        LOG_WARN("%s rc - %d", ERR_SQL, rc);
    }

    sqlite3_bind_text(stmt, 1, item.name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, item.att);
    sqlite3_bind_int(stmt, 3, item.def);
    sqlite3_bind_int(stmt, 4, item.slots);
    sqlite3_bind_int(stmt, 5, item.range);
    sqlite3_bind_int(stmt, 6, item.radius);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOG_WARN("%s insert failed: %s\n", ERR_SQL, sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    log_close();
}

int count_items(void) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    log_init("../log.txt", LOG_LEVEL_DEBUG);

    int count = -1;
    int rc = sqlite3_open("../data/data.db", &db);
    if (rc != SQLITE_OK) {
        LOG_WARN("%s rc - %d", ERR_SQL, rc);
    }

    const char *sql = "SELECT COUNT(id) FROM items;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        LOG_WARN("%s rc - %d", ERR_SQL, rc);
    }

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    } else {
        sqlite3_close(db);
        LOG_WARN("%s count_items: step failed: %s\n", ERR_SQL, sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    log_close();

    return count;
}

void get_items(void) {
    item_list.count = count_items();

    if (item_list.count < 0) LOG_ERROR("%s count_items failed", ERR_SQL);


    item_list.items = calloc(item_list.count, sizeof(ITEM));
    if (!item_list.items) LOG_ERROR("%s calloc failed", ERR_SQL);

    sqlite3 *db;
    sqlite3_stmt *stmt;
    log_init("../log.txt", LOG_LEVEL_DEBUG);

    int rc = sqlite3_open("../data/data.db", &db);
    if (rc != SQLITE_OK) {
        LOG_WARN("%s rc - %d", ERR_SQL, rc);
    }

    const char *sql = "SELECT name, attack, defense, slots, range, radius FROM items;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOG_ERROR("%s load_items: prepare failed: %s\n", ERR_SQL, sqlite3_errmsg(db));
        free(item_list.items);
    }

    int idx = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *txt = sqlite3_column_text(stmt, 1);
        strcpy(item_list.items[idx].name, (const char*)txt);
        item_list.items[idx].att  = sqlite3_column_int(stmt, 2);
        item_list.items[idx].def = sqlite3_column_int(stmt, 3);
        item_list.items[idx].slots   = sqlite3_column_int(stmt, 4);
        item_list.items[idx].range   = sqlite3_column_double(stmt, 5);
        item_list.items[idx].radius  = sqlite3_column_double(stmt, 6);
        idx++;
    }

    if (rc != SQLITE_DONE) {
        LOG_ERROR("%s load_items: step failed: %s\n", ERR_SQL, sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    log_close();
}