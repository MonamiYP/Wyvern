#include "core/Logger.hpp"

void Logger::output(LogLevel level, const char* message, va_list args) {
    const char* levelStr[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG"};
    const char* color[] = { 
        "\e[0;31m", // red
        "\e[0;31m",   // red
        "\e[0;34m",   // orange
        "\e[0;32m",   // green
        "\e[0;36m"    // cyan
    };
    const char* resetColor = "\e[0;37m"; // white

    std::printf("%s[%s]%s ", color[level], levelStr[level], resetColor);
    std::vprintf(message, args);
    std::printf("\n");
}

void Logger::log(LogLevel level, const char* message, ...) {
    va_list args;
    va_start(args, message);
    output(level, message, args);
    va_end(args);
}

void Logger::fatal(const char* message, ...) {
    va_list args;
    va_start(args, message);
    output(LogLevel::LOG_LEVEL_FATAL, message, args);
    va_end(args);
}

void Logger::error(const char* message, ...) {
    va_list args;
    va_start(args, message);
    output(LogLevel::LOG_LEVEL_ERROR, message, args);
    va_end(args);
}

void Logger::info(const char* message, ...) {
    va_list args;
    va_start(args, message);
    output(LogLevel::LOG_LEVEL_INFO, message, args);
    va_end(args);
}

void Logger::debug(const char* message, ...) {
    va_list args;
    va_start(args, message);
    output(LogLevel::LOG_LEVEL_DEBUG, message, args);
    va_end(args);
}