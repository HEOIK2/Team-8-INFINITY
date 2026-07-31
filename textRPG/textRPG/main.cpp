#include <iostream>
#include <vector>
#include "Player.h"
#include "monster.h"
#include "battle.h"
#include "Item.h"
#include <cstdlib>
#include <ctime>

int main() {
    
    srand((unsigned int)time(NULL));

    Player* player = new Player("테스트유저", Job::Cleaner);
    Monster* monster = CreateMonster(MonsterType::PAPER, player->GetLevel());

    std::vector<std::pair<Item, int>> items;
    
    items.push_back({ Item("네오디뮴 자석 석궁 (B)", "자력을 띤 석궁 장치입니다.", "철컥! 캔과 알루미늄을 강하게 끌어당깁니다!", ItemRarity::B, { MonsterType::ALUMINUM }, { MonsterType::GLASS}, 60, 100), 1 });
    items.push_back({ Item("평범한 가위 (C)", "문구용 가위입니다.", "싹둑! 테이프와 박스를 절단합니다!", ItemRarity::C, { MonsterType::PAPER }, { MonsterType::GLASS}, 30, 25), 1 });
    items.push_back({ Item("나무 젓가락(C)", "기본 무기", "나무 젓가락으로 공격!", ItemRarity::D, { MonsterType::PAPER }, { MonsterType::IRON }, 10, 5), 1 });
    items.push_back({ Item("포션", "체력 회복", "포션을 마셨다!", ItemRarity::D, {}, {}, 0, 10, ItemCategory::CONSUMABLE, 50), 3 });

    StartBattle(player, monster, items);

    delete player;
    delete monster;
    return 0;
}
