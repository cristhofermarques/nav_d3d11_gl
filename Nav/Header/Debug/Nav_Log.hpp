#ifndef __NAV_LOG_HPP__
#define __NAV_LOG_HPP__

#include <Api/Nav_Api.hpp>
#include <UI/Console/Nav_Console.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>

#define FLOAT_LOG_FORMAT "%f "
#define DOUBLE_LOG_FORMAT "%lf "
#define CHAR_LOG_FORMAT "%c "
#define STRING_LOG_FORMAT "%s "
#define DECIMAL_LOG_FORMAT "%d "
#define HEXADECIMAL_LOG_FORMAT "%x "

#ifdef NAV_BUILD_MODE_DEBUG

#include <stdio.h>
#include <stdlib.h>

#define LOG_ERROR(msg) \
    Console::SetPrintColor(Console::PrintColor::Red); \
    printf("ERROR %s %s() %d : " STRING_LOG_FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, msg); \
    Console::SetPrintColor(Console::PrintColor::White);

#define LOG_SUCCESS(msg) \
    Console::SetPrintColor(Console::PrintColor::Green); \
    printf("SUCCESS %s %s() %d : " STRING_LOG_FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, msg); \
    Console::SetPrintColor(Console::PrintColor::White);

#define LOG_INFO(msg) \
    Console::SetPrintColor(Console::PrintColor::Yellow); \
    printf("INFO %s %s() %d : " STRING_LOG_FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, msg); \
    Console::SetPrintColor(Console::PrintColor::White);

#define LOG_MSG(msg) \
    Console::SetPrintColor(Console::PrintColor::White); \
    printf("MSG %s %s() %d : " STRING_LOG_FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, msg);

#define LOG(log_format, logPrintColor, ...) \
    Console::SetPrintColor(logPrintColor); \
    printf("LOG %s %s() %d : " log_format "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    Console::SetPrintColor(Console::PrintColor::White);

#else

#define LOG_SUCCESS(error_msg)
#define LOG_ERROR(error_msg)
#define LOG_INFO(info_msg)
#define LOG_MSG(info_msg)
#define LOG(log_format, log_print_color, ...)

#endif

#endif