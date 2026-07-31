#pragma once
#include <iostream>
#include <string>
#include "type.h"

class Player;

class Monster {
private:
    std::string name;     // 몬스터 이름
    MonsterType property; // 몬스터 속성 (type.h에 정의됨)
    int hp;
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

    int getAtk() { return atk; }
    void setAtk(int atk) { this->atk = atk; }

    int getExpReward() { return expReward; }
    int getGoldReward() { return goldReward; }

    void Attack(Player* target);
};

Monster* CreateMonster(MonsterType, int playerlevel);