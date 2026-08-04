#pragma once
#include <vector>
#include <utility>
#include <map>
#include <string>

class Player;
class Monster;
class Item;

Monster* StageMonster(int playerLevel, int& selectedStage);

bool StartBattle(Player* player, Monster* monster); // 전투시작

void EnterBattle(Player* player);  //전투처리

void ClearStage(int stage); // 중간, 메인보스 처리를 위한 스테이지 클리어

void EnterEnding(Player* player); // 엔딩

void ResetBattleStats();

std::map<std::string, int> GetMonsterKillCount();