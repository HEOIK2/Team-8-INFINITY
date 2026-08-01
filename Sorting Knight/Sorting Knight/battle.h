#pragma once
#include <string>

class Player;
class Monster;
class ItemManager;

// 전투 결과. main.cpp가 이 값을 보고 게임오버/엔딩 처리를 한다.
enum class BattleResult {
    PLAYER_WIN,
    PLAYER_LOSE
};

// 전투 시작 (기획서 8번 항목 구현)
//   - 공격 수단은 인벤토리의 아이템뿐 (기본공격 없음 - 기획서 차별화 ④)
//   - 무기는 사용해도 소모되지 않고, 소비 아이템만 소모됨 (기획서 5번)
//   - 데미지 = (플레이어 ATK + 아이템 ATK) × 속성 배수 1.5 (기획서 8번 권장안)
//   - 승리 시 보상(EXP/골드/30% 드랍)과 처치 집계까지 여기서 처리
//   - placeName은 화면 제목에 표시할 장소 이름 ("쓰레기장" 등)
BattleResult StartBattle(Player* player, Monster* monster, ItemManager& itemManager, const std::string& placeName);
