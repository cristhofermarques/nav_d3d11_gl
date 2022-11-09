#ifndef __NAV_PROFILE_HPP__
#define __NAV_PROFILE_HPP__

#include <chrono>
#include "Nav_Debug.hpp"

typedef std::chrono::steady_clock::time_point TimePoint;

#define HOUR std::chrono::hours
#define SECOND std::chrono::seconds
#define MILLISECOND std::chrono::milliseconds
#define MICROSECOND std::chrono::microseconds
#define NANOSECOND std::chrono::nanoseconds

#define NOW() std::chrono::high_resolution_clock::now()
#define GET_ELAPSED(type, timePoint) std::chrono::duration_cast<type>(NOW() - timePoint).count()
#define GET_HOUR_ELAPSED(timePoint) GET_ELAPSED(HOUR, timePoint)
#define GET_SECOND_ELAPSED(timePoint) GET_ELAPSED(SECOND, timePoint)
#define GET_MILLISECOND_ELAPSED(timePoint) GET_ELAPSED(MILLISECOND, timePoint)
#define GET_MICROSECOND_ELAPSED(timePoint) GET_ELAPSED(MICROSECOND, timePoint)
#define GET_NANOSECOND_ELAPSED(timePoint) GET_ELAPSED(NANOSECOND, timePoint)

#define PROFILE_FUNCTION(function, ...) \
    { \
        TimePoint t0 = NOW(); \
        function(__VA_ARGS__); \
        LOG(STRING_LOG_FORMAT DECIMAL_LOG_FORMAT STRING_LOG_FORMAT, Console::White, #function, GET_MILLISECOND_ELAPSED(t0), "ms"); \
    }


#endif