#include <stdarg.h>
#include <stdio.h>

#include "log.h"

#ifdef FLAG_RELEASE
log_level_t log_level = WARNING;
#else
log_level_t log_level = DEBUG;
#endif

const char *message[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

const char *colors[] = {
    "\x1B[94m", // DEBUG   => blue
    "\x1B[00m", // INFO    => default color
    "\x1B[93m", // WARNING => yellow
    "\x1B[91m"  // ERROR   => red
};

void _logging(
    const char * filename, log_level_t level, const char * format, ...)
{
    va_list args;

    if (level < log_level) return;
    if (level < DEBUG || level > ERROR) level = INFO;

    fprintf(stderr, "%s[%s:%s]: ", colors[level], message[level], filename);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\x1B[0m\n");
}
