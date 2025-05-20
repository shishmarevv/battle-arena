#include <stdio.h>
    #include <stdlib.h>

    #include "../include/battle-arena.h"

    #include <math.h>

    /**
     * Returns the maximum of two integers.
     *
     * @param a First integer to compare
     * @param b Second integer to compare
     * @return The larger of the two input integers
     */
    int max(const int a, const int b) {
        return (a > b) ? a : b;
    }

    /**
     * Logs an informational message to both log file and stderr.
     * Opens the log file, writes the message, and then closes it.
     *
     * @param message The informational message to log
     */
    void info(const char *message) {
        log_init("../log.txt", LOG_LEVEL_INFO);
        LOG_INFO("%s\n", message);
        log_close();
    }

    /**
     * Logs an error message, then exits the program.
     * Opens the log file, writes the error message, closes it, and terminates execution.
     *
     * @param message The error message to log
     * @note This function does not return as it calls exit(0)
     */
    void error (const char *message) {
        log_init("../log.txt", LOG_LEVEL_ERROR);
        LOG_ERROR("%s\n", message);
        log_close();
        exit(0);
    }

    /**
     * Logs a warning message to both log file and stderr.
     * Opens the log file, writes the warning message, and then closes it.
     *
     * @param message The warning message to log
     */
    void warning (const char *message) {
        log_init("../log.txt", LOG_LEVEL_ERROR);
        LOG_WARN("%s\n", message);
        log_close();
    }