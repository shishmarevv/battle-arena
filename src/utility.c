#include <stdio.h>
#include <stdlib.h>

#include "../include/utility.h"

#include <math.h>

#include "../include/logger.h"

int max(const int a, const int b) {
    return (a > b) ? a : b;
}

void info(const char *message) {
    log_init("../log.txt", LOG_LEVEL_INFO);
    LOG_INFO("%s\n", message);
    log_close();
}

void error (const char *message) {
    log_init("../log.txt", LOG_LEVEL_ERROR);
    LOG_ERROR("%s\n", message);
    log_close();
    exit(0);
}

void warning (const char *message) {
    log_init("../log.txt", LOG_LEVEL_ERROR);
    LOG_WARN("%s\n", message);
    log_close();
}