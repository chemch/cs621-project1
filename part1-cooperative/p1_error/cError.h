#ifndef ERROR_H
#define ERROR_H

#include "compdetect_coop.h"

/// @brief Print an error message and exit the program.
/// @param message The custom error message.
void fatal_error(const char *message);

/// @brief Log a warning (does not exit).
/// @param message Warning message to print.
void warn(const char *message);

#endif // ERROR_H