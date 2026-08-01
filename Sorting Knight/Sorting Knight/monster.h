#pragma once
#include <iostream>
#include <string>
#include "type.h"
#include "ui.h"   // ToActorDisplay()가 반환하는 ActorDisplay 타입 필요

class Player;

class Monster {
private:
    std::string name;     // 몬스터 이름
    MonsterType property; // 몬스터 속성 (type.h에 정의됨)
    int hp;
    int maxHp;            // 최대 체력 (HP바 표시용, 생성 시 hp와 같게 시작)
    int atk;
    int expReward;
    int goldReward;

public:
    Monster(std::string name, MonsterType property, int hp, int atk, int expReward, int goldReward);

    virtual ~Monster() {};

    std::string getName() { return name; }
    void setName(std::string name) { this->name = name; }

    MonsterType getProperty() { return property; }
    void setProperty(MonsterType property) { this->property = property; }

    int getHp() { return hp; }
    void setHp(int hp) { this->hp = hp; }
    int getMaxHp() { return maxHp; }

    int getAtk() { return atk; }
    void setAtk(int atk) { this->atk = atk; }

    int getExpReward() { return expReward; }
    int getGoldReward() { return goldReward; }

    void Attack(Player* target);

    // 현재 상태를 JRPG UI 화면(ui.cpp)에 그릴 수 있는 ActorDisplay로 변환한다.
    ActorDisplay ToActorDisplay();
};

// ---------------------------------------------------------------------------
// 몬스터 생성 함수들 (기획서 7번 항목)
//   - 일반 몬스터 : 체력 (Lv×20)~(Lv×30), 공격력 (Lv×5)~(Lv×10) 랜덤
//   - 중간보스    : 위 범위의 약 1.25배 (분리수거장)
//   - 최종보스    : 위 범위의 1.5배 (폐기처리장, 종량제봉투 대마왕)
// ---------------------------------------------------------------------------
Monster* CreateMonster(MonsterType type, int playerlevel);
Monster* CreateMidBoss(int playerlevel);
Monster* CreateBoss(int playerlevel);

// 속성 enum -> 화면 표시용 한글 이름 ("종이류", "캔류" 등)
std::string MonsterTypeToKorean(MonsterType type);
