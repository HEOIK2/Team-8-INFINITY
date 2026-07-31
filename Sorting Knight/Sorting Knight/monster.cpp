#include "Player.h"
#include "monster.h"
#include "type.h"
#include <iostream>

Monster::Monster(std::string name, MonsterType property, int hp, int atk, int expReward, int goldReward) {

	this->name = name;
	this->property = property;
	this->hp = hp;
	this->atk = atk;
	this->expReward = expReward;
	this->goldReward = goldReward;
}


Monster* CreateMonster(MonsterType type, int playerlevel) {

	Monster* monster = nullptr;

	switch (type) {

	case MonsterType::NONE:
		monster = new Monster("일반쓰레기", MonsterType::NONE, 20 + 3 * (playerlevel), 10 + 1 * (playerlevel), 10, 5);
		break;

	case MonsterType::PAPER:
		monster = new Monster("종이박스", MonsterType::PAPER, 40 + 3 * (playerlevel), 10 + 1 * (playerlevel), 15, 10);
		break;

	case MonsterType::PLASTIC:
		monster = new Monster("플라스틱", MonsterType::PLASTIC, 45 + 3 * (playerlevel), 10 + 1 * (playerlevel), 20, 20);
		break;

	case MonsterType::GLASS:
		monster = new Monster("깨진맥주잔", MonsterType::GLASS, 40 + 3 * (playerlevel), 20 + 1 * (playerlevel), 20, 15);
		break;

	case MonsterType::ALUMINUM:
		monster = new Monster("알루미늄방망이", MonsterType::ALUMINUM, 120 + 3 * (playerlevel), 30 + 1 * (playerlevel), 50, 40);
		break;

	case MonsterType::IRON:
		monster = new Monster("철", MonsterType::IRON, 150 + 3 * (playerlevel), 40 + 1 * (playerlevel), 100, 70);
		break;
	}
	return monster;

}
	
void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->GetHp() - damage;
		target->SetHp(remainHp);

}