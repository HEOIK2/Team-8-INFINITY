#pragma once
#include <vector>
#include <utility>
class Player;
class Monster;
class Item;

Monster* StageMonster(int playerLevel, int& selectedStage);
bool StartBattle(Player* player, Monster* monster);
void EnterBattle(Player* player);
void ClearStage(int stage);
