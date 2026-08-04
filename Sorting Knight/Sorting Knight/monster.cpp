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

	case MonsterType::NONE:
		// 1렙 기준 HP 170, ATK 28. (최소 3대 타격 필요)
		monster = new Monster("일반쓰레기", MonsterType::NONE, 150 + 20 * (playerlevel), 25 + 3 * (playerlevel), 20, 15);
		break;

	case MonsterType::PAPER:
		// 1렙 기준 HP 225, ATK 34. 
		monster = new Monster("냉장고 종이박스", MonsterType::PAPER, 200 + 25 * (playerlevel), 30 + 4 * (playerlevel), 25, 20);
		break;

	case MonsterType::PLASTIC:
		// 1렙 기준 HP 280, ATK 39. 물약 소모를 강제하는 단단한 탱커형.
		monster = new Monster("오염된 플라스틱 포크", MonsterType::PLASTIC, 250 + 30 * (playerlevel), 35 + 4 * (playerlevel), 30, 25);
		break;

	case MonsterType::GLASS:
		// 1렙 기준 HP 180, ATK 50. 냅두면 요원 체력이 먼저 갈려나가는 유리대포 딜러형.
		monster = new Monster("깨진 맥주잔", MonsterType::GLASS, 160 + 20 * (playerlevel), 45 + 5 * (playerlevel), 30, 25);
		break;

	case MonsterType::ALUMINUM:
		// 중간 보스 (권장 Lv 5). 5렙 기준 HP 600, ATK 85. 상성 무기 없이는 지옥을 봅니다.
		monster = new Monster("찌그러진 알루미늄방망이", MonsterType::ALUMINUM, 400 + 40 * (playerlevel), 50 + 7 * (playerlevel), 70, 50);
		break;

	case MonsterType::IRON:
		// 최종 보스 (권장 Lv 10). 10렙 기준 HP 1400, ATK 170. 영약과 도핑 아이템 필수!
		monster = new Monster("녹슨 쇠파이프", MonsterType::IRON, 800 + 60 * (playerlevel), 70 + 10 * (playerlevel), 150, 100);
		break;
	}
	return monster;

}
	
void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->GetHp() - damage;
		target->SetHp(remainHp);

}