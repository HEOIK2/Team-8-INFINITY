#include "Item.h"

// 생성자 구현
Item::Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
    std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price,
    ItemCategory category, int healHP, int buffATK, int buffMaxHP, int gainLevel, bool isPermanent)
    : name(name), description(desc), attackText(attackText), rarity(rarity),
    strongAgainst(strong), weakAgainst(weak), baseATK(ATK), price(price),
    category(category), healHP(healHP), buffATK(buffATK), buffMaxHP(buffMaxHP), gainLevel(gainLevel), isPermanent(isPermanent) {
}

std::string Item::GetName() const { return name; }
std::string Item::GetDescription() const { return description; }
std::string Item::GetAttackText() const { return attackText; }
ItemRarity Item::GetRarity() const { return rarity; }
std::vector<MonsterType> Item::GetStrongAgainst() const { return strongAgainst; }
std::vector<MonsterType> Item::GetWeakAgainst() const { return weakAgainst; }
int Item::GetBaseATK() const { return baseATK; }
int Item::GetPrice() const { return price; }

std::string Item::GetRarityString() const {
    switch (rarity) {
    case ItemRarity::C: return "C";
    case ItemRarity::B: return "B";
    case ItemRarity::A: return "A";
    case ItemRarity::S: return "S";
    case ItemRarity::N: return "N";
    default: return "?";
    }
}

ItemCategory Item::GetCategory() const { return category; }
int Item::GetHealHP() const { return healHP; }
int Item::GetBuffATK() const { return buffATK; }
int Item::GetBuffMaxHP() const { return buffMaxHP; }
int Item::GetGainLevel() const { return gainLevel; } // 🌟 Getter 구현 변경
bool Item::GetIsPermanent() const { return isPermanent; }

// 속성(enum)을 한글 문자열로 변환해 주는 헬퍼 함수
std::string MonsterTypeToString(MonsterType type) {
    switch (type) {
    case MonsterType::PAPER: return "종이";
    case MonsterType::PLASTIC: return "플라스틱";
    case MonsterType::GLASS: return "유리";
    case MonsterType::ALUMINUM: return "알루미늄";
    case MonsterType::IRON: return "철";
    default: return "";
    }
}

// 무기용: [강: 철,유리 | 약: 종이] 형태로 반환
std::string Item::GetPropertyString() const {
    if (category == ItemCategory::CONSUMABLE) return "";

    std::string res = "<강:";
    if (strongAgainst.empty()) res += "없음";
    else {
        for (size_t i = 0; i < strongAgainst.size(); ++i) {
            res += MonsterTypeToString(strongAgainst[i]);
            if (i + 1 < strongAgainst.size()) res += ",";
        }
    }
    res += "|약:";
    if (weakAgainst.empty()) res += "없음";
    else {
        for (size_t i = 0; i < weakAgainst.size(); ++i) {
            res += MonsterTypeToString(weakAgainst[i]);
            if (i + 1 < weakAgainst.size()) res += ",";
        }
    }
    res += ">";
    return res;
}

// 소비템용: <HP회복+50> 형태로 반환
std::string Item::GetEffectString() const {
    if (category == ItemCategory::WEAPON) return "";

    std::string res = "<";
    if (healHP > 0) res += "HP+" + std::to_string(healHP) + " ";
    if (buffATK > 0) res += (isPermanent ? "최대공격력+" : "공격력+") + std::to_string(buffATK) + " ";
    if (buffMaxHP > 0) res += "최대HP+" + std::to_string(buffMaxHP) + " ";
    if (gainLevel > 0) res += "레벨+" + std::to_string(gainLevel) + " ";

    if (res.back() == ' ') res.pop_back(); // 마지막 공백 제거
    res += ">";
    return res;
}