#pragma once

#include <string>
#include <vector>
#include "type.h"  

enum class ItemCategory { WEAPON, CONSUMABLE };


class Item {

private: std::string name;
	   std::string description;
	   std::string attackText;
	   ItemRarity rarity;
	   std::vector<MonsterType> strongAgainst; std::vector<MonsterType> weakAgainst;
	   int baseATK;
	   int price;
	   ItemCategory category;

	   // 소비템 효과 변수들
	   int healHP;        // 1. 전투 중 체력 회복량
	   int buffATK;       // 2 & 4. 공격력 증가량 (isPermanent에 따라 일시적/영구적 결정)
	   int buffMaxHP;     // 3. 최대 체력 증가량
	   int gainLevel;     // 5. 레벨 업 수치 (기존 gainEXP에서 변경)
	   bool isPermanent;  // 영구 스탯 적용 여부 

public:
	//생성자
	Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity, std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price, ItemCategory category = ItemCategory::WEAPON, int healHP = 0, int buffATK = 0, int buffMaxHP = 0, int gainLevel = 0, bool isPermanent = false);

	std::string GetName() const;
	std::string GetDescription() const;
	std::string GetAttackText() const;
	// 소비템의 경우 "사용 시 출력 텍스트"로 쓸 수 있습니다. 
	ItemRarity GetRarity() const; std::vector<MonsterType> GetStrongAgainst() const;
	std::vector<MonsterType> GetWeakAgainst() const;
	int GetBaseATK() const;
	int GetPrice() const;
	std::string GetRarityString() const;
	ItemCategory GetCategory() const;
	int GetHealHP() const;
	int GetBuffATK() const;
	int GetBuffMaxHP() const;
	int GetGainLevel() const;
	bool GetIsPermanent() const;
};