#pragma once
#include <string>
#include <vector>
#include "type.h" 

// 아이템 종류 구분
enum class ItemCategory {
    WEAPON,     // 장착용 (무기)
    CONSUMABLE  // 사용용 (소비템)
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

    // 소비템 효과 변수들
    int healHP;        // 1. 전투 중 체력 회복량
    int buffATK;       // 2 & 4. 공격력 증가량 (isPermanent에 따라 일시적/영구적 결정)
    int buffMaxHP;     // 3. 최대 체력 증가량
    int gainLevel;     // 5. 레벨 업 수치 (기존 gainEXP에서 변경)
    bool isPermanent;  // 영구 스탯 적용 여부

public:
    // 생성자 (기본값 설정)
    Item(std::string name, std::string desc, std::string attackText, ItemRarity rarity,
        std::vector<MonsterType> strong, std::vector<MonsterType> weak, int ATK, int price,
        ItemCategory category = ItemCategory::WEAPON,
        int healHP = 0, int buffATK = 0, int buffMaxHP = 0, int gainLevel = 0, bool isPermanent = false);

	/* 공격용 아이템 생성자 예시 
    1.이름 2.설명 3.공격 텍스트 4.등급 5.강점 속성 6.약점 속성 7.기본 공격력 8.가격
    Item(
    "평범한 가위 (C)", "문구용 가위입니다.",
        "싹둑! 테이프와 박스를 절단합니다!",
        ItemRarity::C, { MonsterType::PAPER }, { MonsterType::GLASS, MonsterType::IRON }, 30, 25
        )
     */

	 /* 소비용 아이템 생성자 예시
	 * 1.이름 2.설명 3.사용 텍스트 4.등급 5.강점 속성 6.약점 속성 7.기본 공격력 8.가격 9.카테고리 10.체력 회복량 11.공격력 증가량 12.최대 체력 증가량 13.레벨 업 수치 14.영구 스탯 적용 여부
     Item(
            "기적의 엘릭서 (N)", "전설 속의 영약으로 모든 능력이 비약적으로 상승합니다.",
            "몸이 완벽하게 재구성됩니다! (최대체력 200, 기본 공격력 50 영구 증가)",
            ItemRarity::N, {}, {}, 0, 1000,
            ItemCategory::CONSUMABLE, 0, 50, 200, 0, true // buffATK 50, buffMaxHP 200, isPermanent true
        )
     */

    std::string GetName() const;
    std::string GetDescription() const;
    std::string GetAttackText() const;
    ItemRarity GetRarity() const;
    std::vector<MonsterType> GetStrongAgainst() const;
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