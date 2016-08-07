#ifndef LOG_H_
#define LOG_H_

typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} log_level_t;

extern log_level_t log_level;

void _logging(
    const char *filename, log_level_t level, const char * format, ...);

/**
    Log a message into stderr

    @param level: log level, DEBUG, INFO, etc...
    @param ...: log message with arguments, same interface as `printf`.
    @return none.
*/
#define logging(level, ...) _logging(__FILE__, (level), __VA_ARGS__)

#endif // LOG_H_
