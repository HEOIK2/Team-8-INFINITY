#include "Item.h"

Item::Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
    std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price,
    ItemCategory category, int healHP, int buffATK, int buffMaxHP, int gainLevel, bool isPermanent)
    : name(name)
    , description(desc)
    , attackText(attackText)
    , rarity(rarity)
    , strongAgainst(strong)
    , weakAgainst(weak)
    , baseATK(ATK)
    , price(price)
    , category(category)
    , healHP(healHP)
    , buffATK(buffATK)
    , buffMaxHP(buffMaxHP)
    , gainLevel(gainLevel)
    , isPermanent(isPermanent)
{
}

std::string Item::GetName() const { return name; }
std::string Item::GetDescription() const { return description; }
std::string Item::GetAttackText() const { return attackText; }
ItemRarity Item::GetRarity() const { return rarity; }
std::vector<MonsterType> Item::GetStrongAgainst() const { return strongAgainst; }
std::vector<MonsterType> Item::GetWeakAgainst() const { return weakAgainst; }
int Item::GetBaseATK() const { return baseATK; }
int Item::GetPrice() const { return price; }
ItemCategory Item::GetCategory() const { return category; }
int Item::GetHealHP() const { return healHP; }
int Item::GetBuffATK() const { return buffATK; }
int Item::GetBuffMaxHP() const { return buffMaxHP; }
int Item::GetGainLevel() const { return gainLevel; }
bool Item::GetIsPermanent() const { return isPermanent; }

std::string Item::GetRarityString() const
{
    switch (rarity)
    {
    case ItemRarity::D: return "D";
    case ItemRarity::C: return "C";
    case ItemRarity::B: return "B";
    case ItemRarity::A: return "A";
    case ItemRarity::S: return "S";
    default: return "";
    }
}
