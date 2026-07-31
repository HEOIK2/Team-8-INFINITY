#pragma once
#include <string>
#include <vector>
#include "Types.h" 

enum class ItemCategory {
    WEAPON,    
    CONSUMABLE  
};

class Item {
private:
    std::string name;
    std::string description;
    std::string attackText;
    ItemRarity rarity;
    std::vector<MonsterType> strongAgainst;
    std::vector<MonsterType> weakAgainst;
    int baseATK;
    int price;
    ItemCategory category;

public:
    // 끝에 category와 healAmount를 추가하고 기본값을 줍니다.
    Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
        std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price,
        ItemCategory category = ItemCategory::WEAPON);

    // 기존 Getters
    std::string GetName() const;
    std::string GetDescription() const;
    std::string GetAttackText() const; // 소비템의 경우 "사용 시 출력 텍스트"로 쓸 수 있습니다.
    ItemRarity GetRarity() const;
    std::vector<MonsterType> GetStrongAgainst() const;
    std::vector<MonsterType> GetWeakAgainst() const;
    int GetBaseATK() const;
    int GetPrice() const;
    std::string GetRarityString() const;
    ItemCategory GetCategory() const;
    int GetHealAmount() const;
};