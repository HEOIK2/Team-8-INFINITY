#pragma once

#include <string>
#include <vector>

namespace GameLog
{
    void Add(const std::string& text);
    std::vector<std::string> GetAll();
}