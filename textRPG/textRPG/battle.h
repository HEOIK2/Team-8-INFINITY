#pragma once
#include <vector>
#include <utility>
class Player;
class Monster;
class Item;

void StartBattle(Player* player, Monster* monster, std::vector<std::pair<Item, int>>& items);
