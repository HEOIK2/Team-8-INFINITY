#include "Player.h"
#include "monster.h"
#include "type.h"
#include <vector>
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
		// 1렙 기준 골드 30. 레벨업마다 15씩 증가.
		monster = new Monster("일반쓰레기", MonsterType::NONE,
			150 + 20 * (playerlevel), 25 + 3 * (playerlevel),
			20 + 5 * (playerlevel), 15 + 15 * (playerlevel));
		break;

	case MonsterType::PAPER:
		// 1렙 기준 골드 40. 레벨업마다 20씩 증가.
		monster = new Monster("냉장고 종이박스", MonsterType::PAPER,
			200 + 25 * (playerlevel), 30 + 4 * (playerlevel),
			25 + 6 * (playerlevel), 20 + 20 * (playerlevel));
		break;

	case MonsterType::PLASTIC:
		// 1렙 기준 골드 50. 포션을 강제하는 탱커라 보상을 두둑하게 줍니다.
		monster = new Monster("오염된 플라스틱 포크", MonsterType::PLASTIC,
			250 + 30 * (playerlevel), 35 + 4 * (playerlevel),
			30 + 8 * (playerlevel), 25 + 25 * (playerlevel));
		break;

	case MonsterType::GLASS:
		// 1렙 기준 골드 45. 공격력이 쎄서 위험한 만큼 보상도 높습니다.
		monster = new Monster("깨진 맥주잔", MonsterType::GLASS,
			160 + 20 * (playerlevel), 45 + 5 * (playerlevel),
			30 + 8 * (playerlevel), 25 + 20 * (playerlevel));
		break;

	case MonsterType::ALUMINUM:
		// 중간 보스 (권장 Lv 5). 5렙 기준 골드 300. 
		monster = new Monster("찌그러진 알루미늄방망이", MonsterType::ALUMINUM,
			400 + 40 * (playerlevel), 50 + 7 * (playerlevel),
			70 + 20 * (playerlevel), 50 + 50 * (playerlevel));
		break;

	case MonsterType::IRON:
		// 최종 보스 (권장 Lv 10). 10렙 기준 골드 1100.
		monster = new Monster("전손 소나타", MonsterType::IRON,
			800 + 60 * (playerlevel), 70 + 10 * (playerlevel),
			150 + 40 * (playerlevel), 100 + 100 * (playerlevel));
		break;
	}
	return monster;

}
	
void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->GetHp() - damage;
		target->SetHp(remainHp);

}

// 새로 만드는 함수: 몬스터 이름에 맞는 아스키 아트 반환
std::vector<std::string> GetMonsterArt(const std::string& name) {
    if (name == "일반쓰레기") {
        return {
            "                                     /\\  /\\                                     ",
            "                                    /  \\/  \\                                    ",
            "                                  .-'       '-.                                 ",
            "                                 /   (o) (o)   \\                                ",
            "                                |     <___>     |                               ",
            "                               /  [ 50L TRASH ]  \\                              ",
            "                              |   .-----------.   |                             ",
            "                             /   /  WARNING!   \\   \\                            ",
            "                            |   |  NON-RECYCLE  |   |                           ",
            "                             \\   \\             /   /                            ",
            "                              '--'------------'--'                              ",
            "                             /                      \\                           ",
            "                            (________________________)                          ",
            "                            ~~~~~~~~~~~~~~~~~~~~~~~~~~                          "
        };
    }
    if (name == "냉장고 종이박스") {
        return {
            "                             .------------------------.                         ",
            "                            /                        /|                         ",
            "                           /   FRIDGE PACKAGING     / |                         ",
            "                          +------------------------+  |                         ",
            "                          |   .----------------.   |  |                         ",
            "                          |   |  (o)      (o)  |   |  |                         ",
            "                          |   |     __||__     |   |  |                         ",
            "                          |   |    /      \\    |   |  |                         ",
            "                          |   |   |  ||||  |   |   |  |                         ",
            "                          |   '----------------'   |  +                         ",
            "                          |   [THIS SIDE UP ^]     | /                          ",
            "                          |   |== HEAVY CARDBOARD==|/                           ",
            "                          '---================----'                             ",
            "                             / /            \\ \\                                 "
        };
    }
    if (name == "오염된 플라스틱 포크") {
        return {
            "                               /\\   /\\   /\\   /\\                                ",
            "                              |  | |  | |  | |  |                               ",
            "                              |  | |  | |  | |  |                               ",
            "                              |  | |  | |  | |  |                               ",
            "                              |  '-'  '-'  '-'  |                               ",
            "                              |   (o)      (o)  |                               ",
            "                               \\      __       /                                ",
            "                                \\    /__\\     /                                 ",
            "                                 '--._____.--'                                  ",
            "                                    |     |                                     ",
            "                                    |DIRTY|                                     ",
            "                                    |FORK |                                     ",
            "                                    |     |                                     ",
            "                                    |_____|                                     "
        };
    }
    if (name == "깨진 맥주잔") {
        return {
            "                              ___            ___                                ",
            "                             /   \\          /   \\                               ",
            "                            |     |  /\\/\\  |     |                              ",
            "                             \\___/  /    \\  \\___/                               ",
            "                             | C / A \\ S / S |                                  ",
            "                             |  /  \\  \\_/  | |  -- (CRACKED!)                   ",
            "                             | / (o)  (o) \\| |                                  ",
            "                             |/     --     \\ |                                  ",
            "                             |  /\\/\\/\\/\\/\\  | |                                 ",
            "                             | /          \\ | |                                 ",
            "                             |/            \\| |                                 ",
            "                             |______________|/                                  ",
            "                             /              \\                                   ",
            "                            (________________)                                  "
        };
    }
    if (name == "찌그러진 알루미늄방망이") {
        return {
            "                                     .---.                                      ",
            "                                    / (o) \\                                     ",
            "                                   |   _   |                                    ",
            "                                    \\ (o) /                                     ",
            "                                     '---'                                      ",
            "                                    /     \\                                     ",
            "                                   /   /\\  \\   <-- [CRUSHED!]                   ",
            "                                  (   /  \\  )                                   ",
            "                                   \\  \\  / /                                    ",
            "                                    \\  \\/ /                                     ",
            "                                     |   |                                      ",
            "                                     |   |                                      ",
            "                                     |===|                                      ",
            "                                     (___)                                      "
        };
    }
    if (name == "전손 소나타") {
        return {
            "                                 .--------.                                     ",
            "                            ____/          \\____                                ",
            "                          .'  [SONATA] (x)(x)   '.                              ",
            "                         /  __  ______  ______    \\                             ",
            "                        |  /  \\|  __  ||  __  |    |                            ",
            "                        | | () ||    ||||    ||    |                            ",
            "  ======================|  \\__/|______||______|    |==========================  ",
            " ( [TOTAL LOSS]         |     ||          ||       |                          ) ",
            "  ======================\\_____||__________||______/===========================  ",
            "                         /  --  /========\\  --  \\                               ",
            "                        |  (O)  |  RUST  |  (O)  |                              ",
            "                         \\_____/\\________/\\_____/                               ",
            "                        ~~~~~~~~~~~~~~~~~~~~~~~~~~                              ",
            "                       [ ENGINE SMOKE / TOTALED ]                               "
        };
    }
    // 매칭 안 되면 빈 벡터 (아트 없이 진행)
    return {};
}

