#include "Player.h"
#include "monster.h"
#include "type.h"
#include <iostream>

Monster::Monster(std::string name, MonsterType property, int hp, int atk, int expReward, int goldReward) {

	this->name = name;
	this->property = property;
	this->hp = hp;
	this->maxHp = hp;
	this->atk = atk;
	this->expReward = expReward;
	this->goldReward = goldReward;
}


Monster* CreateMonster(MonsterType type, int playerlevel) {

	Monster* monster = nullptr;

	switch (type) {

	case MonsterType::NONE: // 동네북 일반쓰레기
		monster = new Monster("일반쓰레기", MonsterType::NONE, 30 + 10 * (playerlevel), 10 + 2 * (playerlevel), 15, 10);
		break;

	case MonsterType::PAPER:
		monster = new Monster("냉장고 종이박스", MonsterType::PAPER, 50 + 12 * (playerlevel), 12 + 2 * (playerlevel), 20, 15);
		break;

	case MonsterType::PLASTIC: // 체력이 약간 높은 탱커형
		monster = new Monster("오염된 플라스틱 포크", MonsterType::PLASTIC, 60 + 15 * (playerlevel), 15 + 3 * (playerlevel), 25, 20);
		break;

	case MonsterType::GLASS: // 체력은 낮지만 공격력이 높은 딜러형
		monster = new Monster("깨진 맥주잔", MonsterType::GLASS, 50 + 12 * (playerlevel), 20 + 4 * (playerlevel), 25, 20);
		break;

	case MonsterType::ALUMINUM: // 중간 보스 급 (권장 Lv 5)
		monster = new Monster("찌그러진 알루미늄방망이", MonsterType::ALUMINUM, 100 + 25 * (playerlevel), 25 + 5 * (playerlevel), 50, 40);
		break;

	case MonsterType::IRON: // 최종 보스 급 (권장 Lv 10)
		monster = new Monster("녹슨 쇠파이프", MonsterType::IRON, 200 + 40 * (playerlevel), 40 + 7 * (playerlevel), 100, 70);
		break;
	}
	return monster;

}
	
void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->GetHp() - damage;
		target->SetHp(remainHp);

}