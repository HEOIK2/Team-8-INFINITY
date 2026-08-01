#include "Player.h"
#include "monster.h"
#include "type.h"
#include "asciiArt.h"
#include <iostream>
#include <cstdlib>

Monster::Monster(std::string name, MonsterType property, int hp, int atk, int expReward, int goldReward) {

	this->name = name;
	this->property = property;
	this->hp = hp;
	this->maxHp = hp;
	this->atk = atk;
	this->expReward = expReward;
	this->goldReward = goldReward;
}

namespace {
	// lo ~ hi 사이의 정수 랜덤 (양 끝 포함)
	int RandBetween(int lo, int hi) {
		if (hi <= lo) return lo;
		return lo + rand() % (hi - lo + 1);
	}
}

// ---------------------------------------------------------------------------
// CreateMonster - 일반 몬스터 생성 (기획서 7번)
//   속성별 몬스터 2종(금속 계열은 1종씩) 중 랜덤으로 이름을 고르고,
//   스탯은 플레이어 레벨에 비례한 랜덤값으로 만든다.
//     체력   : (Lv × 20) ~ (Lv × 30)
//     공격력 : (Lv × 5)  ~ (Lv × 10)
//   보상은 공식 스펙 고정: EXP 50, Gold 10~20 랜덤
// ---------------------------------------------------------------------------
Monster* CreateMonster(MonsterType type, int playerlevel) {

	int lv = (playerlevel < 1) ? 1 : playerlevel;
	int hp = RandBetween(lv * 20, lv * 30);
	int atk = RandBetween(lv * 5, lv * 10);
	int exp = 50;
	int gold = RandBetween(10, 20);

	std::string name;
	switch (type) {
	case MonsterType::NONE:
		name = (rand() % 2 == 0) ? "음식물 찌꺼기" : "스티로폼 덩어리";
		break;
	case MonsterType::PAPER:
		name = (rand() % 2 == 0) ? "테이프 안 뗀 박스" : "젖은 신문지 더미";
		break;
	case MonsterType::PLASTIC:
		name = (rand() % 2 == 0) ? "뭉쳐진 비닐봉지" : "뜯긴 과자봉지";
		break;
	case MonsterType::GLASS:
		name = (rand() % 2 == 0) ? "깨진 소주병" : "라벨 붙은 유리병";
		break;
	case MonsterType::ALUMINUM:
		name = "찌그러진 캔뭉치";
		break;
	case MonsterType::IRON:
		name = "녹슨 통조림";
		break;
	default:
		name = "정체불명의 쓰레기";
		break;
	}

	return new Monster(name, type, hp, atk, exp, gold);
}

// 중간보스 (분리수거장) - 일반 몬스터의 약 1.25배 스탯
Monster* CreateMidBoss(int playerlevel) {
	int lv = (playerlevel < 1) ? 1 : playerlevel;
	int hp = RandBetween(lv * 25, lv * 37);
	int atk = RandBetween(lv * 6, lv * 12);
	return new Monster("소각로의 군주", MonsterType::IRON, hp, atk, 100, 50);
}

// 최종보스 (폐기처리장) - 일반 몬스터의 1.5배 스탯, 무속성(상성 안 통함)
Monster* CreateBoss(int playerlevel) {
	int lv = (playerlevel < 1) ? 1 : playerlevel;
	int hp = RandBetween(lv * 30, lv * 45);
	int atk = RandBetween(lv * 7, lv * 15);
	return new Monster("종량제봉투 대마왕", MonsterType::NONE, hp, atk, 200, 100);
}

std::string MonsterTypeToKorean(MonsterType type) {
	switch (type) {
	case MonsterType::PAPER:    return "종이류";
	case MonsterType::PLASTIC:  return "플라스틱류";
	case MonsterType::GLASS:    return "유리류";
	case MonsterType::ALUMINUM: return "캔류";
	case MonsterType::IRON:     return "고철류";
	default:                    return "일반쓰레기";
	}
}

void Monster::Attack(Player * target) {
		int damage = this->atk;
		int remainHp = target->GetHp() - damage;
		target->SetHp(remainHp);

}

// 속성(MonsterType)별로 이름 색을 다르게 표시해서 화면에서 몬스터 종류를
// 한눈에 구분할 수 있게 한다.
static UIColor MonsterTypeToColor(MonsterType type) {
	switch (type) {
	case MonsterType::PAPER:    return UIColor::White;
	case MonsterType::PLASTIC:  return UIColor::Cyan;
	case MonsterType::GLASS:    return UIColor::Blue;
	case MonsterType::ALUMINUM: return UIColor::Gray;
	case MonsterType::IRON:     return UIColor::Magenta;
	default:                    return UIColor::Green;
	}
}

ActorDisplay Monster::ToActorDisplay() {
	ActorDisplay display;
	display.art = GameArt::Monster(name);
	display.name = name;
	display.nameColor = MonsterTypeToColor(property);
	display.statLines = {
		{ "[" + MonsterTypeToKorean(property) + "]", MonsterTypeToColor(property) },
		{ "HP " + UI_MakeGauge(hp, maxHp, 10), UIColor::Red },
		{ "ATK " + std::to_string(atk), UIColor::White },
	};
	return display;
}
