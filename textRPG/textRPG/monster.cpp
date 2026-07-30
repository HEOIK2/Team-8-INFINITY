#include "player.h"
#include "monster.h"
#include "type.h"
#include <iostream>

Monster::Monster(std::string name, MonsterType property, int hp, int atk) {

	this->name = name;
	this->property = property;
	this->hp = hp;
	this->atk = atk;
}


Monster* CreateMonster(MonsterType type, int playerlevel) {

	Monster* monster = nullptr;

	switch (type) {

	case MonsterType::NONE:
		monster = new Monster("ÀÏ¹Ý¾²·¹±â", MonsterType::NONE, 20 + 3 * (playerlevel), 10 + 1 * (playerlevel));
		break;

	case MonsterType::PAPER:
		monster = new Monster("Á¾ÀÌ¹Ú½º", MonsterType::PAPER, 40 + 3 * (playerlevel), 10 + 1 * (playerlevel));
		break;

	case MonsterType::PLASTIC:
		monster = new Monster("ÇÃ¶ó½ºÆ½", MonsterType::PLASTIC, 45 + 3 * (playerlevel), 10 + 1 * (playerlevel));
		break;

	case MonsterType::GLASS:
		monster = new Monster("±úÁø¸ÆÁÖÀÜ", MonsterType::GLASS, 40 + 3 * (playerlevel), 20 + 1 * (playerlevel));
		break;

	case MonsterType::ALUMINUM:
		monster = new Monster("¾Ë·ç¹Ì´½¹æ¸ÁÀÌ", MonsterType::ALUMINUM, 120 + 3 * (playerlevel), 30 + 1 * (playerlevel));
		break;

	case MonsterType::IRON:
		monster = new Monster("Ã¶", MonsterType::IRON, 150 + 3 * (playerlevel), 40 + 1 * (playerlevel));
		break;
	}
	return monster;

}
	
void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->getHp() - damage;
		target->setHp(remainHp);

}