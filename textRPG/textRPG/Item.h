#pragma once

#include <string>
#include <vector>
#include "Types.h"

class Item {
private:
    std::string name;          // 아이템(스킬) 이름
    std::string description;   // 아이템 설명
    std::string attackText;    // 공격시 출력 텍스트
    ItemRarity rarity;         // 등급 (C~S)
    std::vector<MonsterType> strongAgainst;  // 강한 상대 (데미지 증가)
    std::vector<MonsterType> weakAgainst;    // 약한 상대 (데미지 감쇄)
    int baseATK;             // 기본 위력
	int price;               // 아이템 가격

public:
    // 생성자 선언
    Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
        std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK , int price);

    // Getters 선언
    std::string GetName() const;
    std::string GetDescription() const;
    std::string GetAttackText() const;
    ItemRarity GetRarity() const;
    std::vector<MonsterType> GetStrongAgainst() const;
    std::vector<MonsterType> GetWeakAgainst() const;
    int GetBaseATK() const;
	int GetPrice() const;

    // 등급 문자열 반환 헬퍼 함수 선언
    std::string GetRarityString() const;
};