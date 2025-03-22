#include "cError.h"

/// @brief Print an error message and exit the program.
/// @param message The custom error message.
void fatal_error(const char *message) {
    fprintf(stderr, "**ERROR**: %s\n", message);
    exit(EXIT_FAILURE);
}

/// @brief Print an error message with errno and exit the program.
/// @param message The custom error message.
void fatal_perror(const char *message) {
    fprintf(stderr, "**ERROR**: %s - %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

/// @brief Log a warning (does not exit).
/// @param message Warning message to print.
void warn(const char *message) {
    fprintf(stderr, "**WARNING**: %s\n", message);
}
