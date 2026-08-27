#ifndef TYPEC_PARSE_INT_H
#define TYPEC_PARSE_INT_H

#include <charconv>
#include <cstdint>
#include <cstring>
#include <system_error>

inline bool ParseNumericInt(const char *buffer, int32_t &out)
{
    if (buffer == nullptr || *buffer == '\0') {
        return false;
    }
    int value = 0;
    const char *last = buffer + std::strlen(buffer);
    auto result = std::from_chars(buffer, last, value);
    if (result.ec != std::errc() || result.ptr != last) {
        return false;
    }
    out = value;
    return true;
}

#endif
