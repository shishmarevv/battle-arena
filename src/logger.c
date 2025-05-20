#include "../include/battle-arena.h"
#include <stdarg.h>
#include <time.h>

/**
 * Global file pointer for the log file
 */
static FILE *log_file = NULL;

/**
 * Current log level that determines which messages get recorded
 */
LogLevel CURRENT_LOG_LEVEL = LOG_LEVEL_DEBUG;

/**
 * Initializes the logging system.
 * Opens the specified log file in append mode and sets the logging level.
 *
 * @param filename Path to the log file
 * @param level Minimum log level to record
 * @return 0 on success, -1 if file could not be opened
 */
int log_init(const char *filename, LogLevel level) {
    CURRENT_LOG_LEVEL = level;
    log_file = fopen(filename, "a");
    return (log_file != NULL) ? 0 : -1;
}

/**
 * Closes the log file if it's open.
 * Should be called when logging is no longer needed or before program exit.
 */
void log_close(void) {
    if (log_file) fclose(log_file);
}

/**
 * Logs a message with the current timestamp if its level is at or above the current log level.
 * The message is written to both the log file (if open) and stderr.
 *
 * @param lvl The log level of this message
 * @param message Format string for the log message
 * @param ... Variable arguments to be formatted into the message
 */
void log_message(LogLevel lvl, const char *message, ...) {
    if (lvl < CURRENT_LOG_LEVEL) return;

    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char timestr[20];
    strftime(timestr, sizeof timestr, "%F %T", &tm);

    va_list args;
    va_start(args, message);
    if (log_file) {
        fprintf(log_file, "[%s] ", timestr);
        vfprintf(log_file, message, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }

    fprintf(stderr, "[%s] ", timestr);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");

    va_end(args);
}