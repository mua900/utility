#ifndef _LOG_H
#define _LOG_H

enum Log_Level {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

void log(Log_Level level, char const * const msg, ...);

#ifndef LOG_SILENCE

#define LOG_INFO(msg)  log(LOG_LEVEL_INFO, msg)
#define LOG_WARN(msg)  log(LOG_LEVEL_WARN, msg)
#define LOG_ERROR(msg) log(LOG_LEVEL_ERROR , msg)

#define LOG_INFOF(msg, ...)  log(LOG_LEVEL_INFO, msg, __VA_ARGS__)
#define LOG_WARNF(msg, ...)  log(LOG_LEVEL_WARN, msg, __VA_ARGS__)
#define LOG_ERRORF(msg, ...) log(LOG_LEVEL_ERROR , msg, __VA_ARGS__)

#else // LOG_SILENCE

#define LOG_INFO(about, msg) ((void)0)
#define LOG_WARN(about, msg) ((void)0)
#define LOG_ERROR(about, msg) ((void)0)

#define LOG_INFOF(about, msg, ...) ((void)0)
#define LOG_WARNF(about, msg, ...) ((void)0)
#define LOG_ERRORF(about, msg, ...) ((void)0)

#endif  // LOG_SILENCE

#ifdef LOG_IMPLEMENTATION

#include <cstdio>
#include <cstdarg>
#include "log.hpp"

#define ANSI_END \033[0m
#define ANSI_RED \033[31;1;1m

void log(Log_Level level, char const * const msg, ...) {
    char formatted_message[1024];
    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_message, sizeof(formatted_message), msg, args);
    va_end(args);

    switch (level) {
    case LOG_LEVEL_INFO: {
        fprintf(stderr, "\033[32;1;1m[INFO]:\033[0m %s\n", formatted_message);
        break;
    }
    case LOG_LEVEL_WARN: {
        fprintf(stderr, "\033[33;1;1m[WARNING]:\033[0m %s\n", formatted_message);
        break;
    }
    case LOG_LEVEL_ERROR: {
        fprintf(stderr, "\033[31;1;1m[ERROR]:\033[0m %s\n", formatted_message);
        break;
    }
    }
}

#undef ANSI_RED
#undef ANSI_END

#endif // LOG_IMPLEMENTATION

#endif // _LOG_H
