#include "Item.h"

// 생성자 구현 (초기화 리스트에 category, healAmount 추가)
Item::Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
    std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price,
    ItemCategory category, int healAmount)
    : name(name), description(desc), attackText(attackText), rarity(rarity),
    strongAgainst(strong), weakAgainst(weak), baseATK(ATK), price(price),
    category(category) { //변수 초기화 추가
}

// ---------------- 기존 Getters ----------------
std::string Item::GetName() const { return name; }
std::string Item::GetDescription() const { return description; }
std::string Item::GetAttackText() const { return attackText; }
ItemRarity Item::GetRarity() const { return rarity; }
std::vector<MonsterType> Item::GetStrongAgainst() const { return strongAgainst; }
std::vector<MonsterType> Item::GetWeakAgainst() const { return weakAgainst; }
int Item::GetBaseATK() const { return baseATK; }
int Item::GetPrice() const { return price; }

//희귀도 문자열 반환
std::string Item::GetRarityString() const {
    switch (rarity) {
    case ItemRarity::C: return "C";
    case ItemRarity::B: return "B";
    case ItemRarity::A: return "A";
    case ItemRarity::S: return "S";
    default: return "?";
    }
}
//아이템 카테고리 반환
ItemCategory Item::GetCategory() const {
    return category;
}
