#pragma once
#include <iostream>
#include <string>
#include "type.h"

class Player;

class Monster {

private:
	std::string name; // 몬스터 이름
	MonsterType property; // 몬스터 특성 ( type.h에서 가져옴)
	int hp;
	int atk;

public:

	Monster(std::string name, MonsterType property, int hp, int atk);

	virtual ~Monster() {};

	std::string getName() { return name; }
	void setName(std::string name) { this->name = name; }

	MonsterType getProperty() { return property; }
	void setProperty(MonsterType property) {this->property = property; }

	int getHp() { return hp; }
	void setHp(int hp) {this->hp = hp; }

	int getAtk() { return atk; }
	void setAtk(int atk) {this->atk = atk; }

	void Attack(Player* target);

};


Monster* CreateMonster(MonsterType, int playerlevel);