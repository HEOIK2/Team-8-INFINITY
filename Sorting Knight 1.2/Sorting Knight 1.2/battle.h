#pragma once
#include <vector>
#include <utility>
class Player;
class Monster;
class Item;

Monster* StageMonster(int playerLevel, int& selectedStage);
bool StartBattle(Player* player, Monster* monster, std::vector<std::pair<Item, int>>& items);
void EnterBattle(Player* player, std::vector<std::pair<Item, int>>& items);
void ClearStage(int stage);
