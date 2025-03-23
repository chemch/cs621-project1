#ifndef ERROR_H
#define ERROR_H

#include "compdetect_alone.h"

/// @brief Print an error message and exit the program.
/// @param message The custom error message.
void fatal_error(const char *message);

/// @brief Print an error message with errno and exit the program.
/// @param message The custom error message.
void fatal_perror(const char *message);

#endif // ERROR_H