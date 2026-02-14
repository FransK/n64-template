#ifndef N64_LOGGER_H
#define N64_LOGGER_H

#include <iostream>
#include <string>

namespace n64::Logger
{
    void error(std::string_view from, std::string_view message)
    {
        std::cerr << "[ " << from << " ] " << message << "\n";
    }
}

#endif // N64_LOGGER_H