#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
} LogLevel;

extern LogLevel CURRENT_LOG_LEVEL;

int  log_init(const char *filename, LogLevel level);

void log_close(void);

void log_message(LogLevel lvl, const char *fmt, ...);


#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, "DEBUG: " fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message(LOG_LEVEL_INFO,  "INFO:  " fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message(LOG_LEVEL_WARN,  "WARN:  " fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, "ERROR: " fmt, ##__VA_ARGS__)

#endif