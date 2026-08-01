#include "gameLog.h"

// 처치 집계 저장소. 이 파일 안에서만 직접 접근하고,
// 외부는 아래 함수들로만 읽고 쓴다.
namespace {
    std::map<std::string, int> killCounts;
}

void GameLog_RecordKill(const std::string& monsterName) {
    killCounts[monsterName]++;
}

const std::map<std::string, int>& GameLog_GetKills() {
    return killCounts;
}

int GameLog_TotalKills() {
    int total = 0;
    for (const auto& kv : killCounts) {
        total += kv.second;
    }
    return total;
}

void GameLog_Reset() {
    killCounts.clear();
}
