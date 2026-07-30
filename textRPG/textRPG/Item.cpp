#include "Item.h"

// 생성자 구현 (초기화 리스트)
Item::Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
    MonsterType strong, MonsterType weak, int power)
    : name(name), description(desc), attackText(attackText), rarity(rarity),
    strongAgainst(strong), weakAgainst(weak), basePower(power) {
}

// Getters 구현
std::string Item::GetName() const {
    return name;
}

std::string Item::GetDescription() const {
    return description;
}

std::string Item::GetAttackText() const {
    return attackText;
}

ItemRarity Item::GetRarity() const {
    return rarity;
}

TargetType Item::GetStrongAgainst() const {
    return strongAgainst;
}

TargetType Item::GetWeakAgainst() const {
    return weakAgainst;
}

int Item::GetBasePower() const {
    return basePower;
}

// 등급(Enum)을 문자열("D", "C" 등)로 변환
std::string Item::GetRarityString() const {
    switch (rarity) {
    case ItemRarity::D: return "D";
    case ItemRarity::C: return "C";
    case ItemRarity::B: return "B";
    case ItemRarity::A: return "A";
    case ItemRarity::S: return "S";
    default: return "?";
    }
}