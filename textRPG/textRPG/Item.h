#pragma once

#include <string>
#include "Types.h"

class Item {
private:
    std::string name;          // 아이템(스킬) 이름
    std::string description;   // 아이템 설명
    std::string attackText;    // 공격시 출력 텍스트
    ItemRarity rarity;         // 등급 (D~S)
    MonsterType strongAgainst;  // 강한 상대 (데미지 증가)
    MonsterType weakAgainst;    // 약한 상대 (데미지 감쇄)
    int basePower;             // 기본 위력

public:
    // 생성자 선언
    Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
        MonsterType strong, MonsterType weak, int power);

    // Getters 선언
    std::string GetName() const;
    std::string GetDescription() const;
    std::string GetAttackText() const;
    ItemRarity GetRarity() const;
    MonsterType GetStrongAgainst() const;
    MonsterType GetWeakAgainst() const;
    int GetBasePower() const;

    // 등급 문자열 반환 헬퍼 함수 선언
    std::string GetRarityString() const;
};