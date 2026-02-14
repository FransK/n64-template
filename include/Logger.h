#ifndef N64_LOGGER_H
#define N64_LOGGER_H

#include <string_view>

namespace n64::Logger
{
    void error(std::string_view from, std::string_view message);
}

#endif // N64_LOGGER_H
