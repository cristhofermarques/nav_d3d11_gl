#ifndef __NAV_LOG_HPP__
#define __NAV_LOG_HPP__

#include <Api/Nav_Api.hpp>
#include <UI/Console/Nav_Console.hpp>
#include <Type/Nav_Value_Type.hpp>

#define FLOAT_LOG_FORMAT "%f "
#define DOUBLE_LOG_FORMAT "%lf "
#define CHAR_LOG_FORMAT "%c "
#define STRING_LOG_FORMAT "%s "
#define DECIMAL_LOG_FORMAT "%d "
#define HEXADECIMAL_LOG_FORMAT "%x "

#ifdef NAV_BUILD_MODE_DEBUG

#include <stdio.h>
#include <stdlib.h>

#define LOG_MSG(log_format, logPrintColor, ...) \
    Console::SetPrintColor(logPrintColor); \
    printf("LOG %s %s() %d : " log_format "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    Console::SetPrintColor(Console::PrintColor::White);

#else // NAV_BUILD_RELEASE

#define DEBUG_INFO(info_msg)
#define DEBUG_ERROR(error_msg)
#define LOG_MSG(log_format, log_print_color, ...)

#define INIT_DEBUG_HISTORY_SAVE(character_capacity)
#define END_DEBUG_HISTORY_SAVE()

#endif // NAV_BUILD_DEBUG

#endif // NAV_DEBUG_H