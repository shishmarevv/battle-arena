#include "../include/logger.h"
#include <stdarg.h>

static FILE *log_file = NULL;
LogLevel CURRENT_LOG_LEVEL = LOG_LEVEL_DEBUG;

int log_init(const char *filename, LogLevel level) {
    CURRENT_LOG_LEVEL = level;
    log_file = fopen(filename, "a");
    return (log_file != NULL) ? 0 : -1;
}

void log_close(void) {
    if (log_file) fclose(log_file);
}

void log_message(LogLevel lvl, const char *fmt, ...) {
    if (lvl < CURRENT_LOG_LEVEL) return;

    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char timestr[20];
    strftime(timestr, sizeof timestr, "%F %T", &tm);

    va_list args;
    va_start(args, fmt);
    if (log_file) {
        fprintf(log_file, "[%s] ", timestr);
        vfprintf(log_file, fmt, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }

    fprintf(stderr, "[%s] ", timestr);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}
