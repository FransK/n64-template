#include "Logger.h"
#include <iostream>

namespace n64::Logger
{
    void error(std::string_view from, std::string_view message)
    {
        std::cerr << "[ " << from << " ] " << message << "\n";
    }
}
