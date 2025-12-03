#pragma once

#include <cstdarg>
#include <cstdio>
#include <stdexcept>

enum LogLevel {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
};

class Logger {
    public:
        static void init() {}
               
        static void log(LogLevel level, const char* message, ...);
        static void fatal(const char* message, ...);
        static void error(const char* message, ...);
        static void warn(const char* message, ...);
        static void info(const char* message, ...);
        static void debug(const char* message, ...);

    private:
        static void output(LogLevel level, const char* message, va_list args);
};