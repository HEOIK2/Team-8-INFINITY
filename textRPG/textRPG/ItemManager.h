#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Item.h"
#include "Types.h"

class ItemManager {
private:
	//게임 내 모든 도감.
	    std::unordered_map<std::string, Item> itemDatabase;
public:
	ItemManager(){
		// [D등급 아이템]
		itemDatabase.insert("평범한 가위" , 
	}