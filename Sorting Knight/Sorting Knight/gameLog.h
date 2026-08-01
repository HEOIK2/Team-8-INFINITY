#pragma once
#include <string>
#include <map>

// ============================================================================
//  gameLog.h / gameLog.cpp  —  게임 로그 (처치 집계)
// ----------------------------------------------------------------------------
//  "어떤 몬스터를 몇 마리 잡았는지"를 map<string,int>로 집계한다.
//  - 전투 승리 시 battle.cpp가 GameLog_RecordKill() 호출
//  - 상태보기 화면과 엔딩에서 GameLog_GetKills()로 목록을 받아 표시
//  - 새 게임 시작 시 GameLog_Reset()으로 초기화
// ============================================================================

void GameLog_RecordKill(const std::string& monsterName);
const std::map<std::string, int>& GameLog_GetKills();
int  GameLog_TotalKills();
void GameLog_Reset();
