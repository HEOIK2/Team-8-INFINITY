#include "gameLog.h"

namespace
{
    std::vector<std::string> g_lines;
}

void GameLog::Add(const std::string& text)
{
    g_lines.push_back(text);
}

std::vector<std::string> GameLog::GetAll()
{
    return g_lines;
}