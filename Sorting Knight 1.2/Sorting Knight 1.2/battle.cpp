#include "battle.h"
#include "Player.h"
#include "monster.h"
#include "Item.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

Monster* StageMonster(int playerLevel) {

    std::cout << "\n=======스테이지 선택=======\n" << std::endl;
    std::cout << "1. 쓰레기장" << std::endl;
    std::cout << "2. 분리수거장( 권장레벨 : 5)" << std::endl;
    std::cout << "3. 폐기처리장( 권장레벨 : 10)" << std::endl;
    std::cout << "입장 구역을 선택해주세요 : ";

    int stageChoice;
    std::cin >> stageChoice;

    MonsterType randomPool[] = { MonsterType::NONE, MonsterType::PAPER, MonsterType::PLASTIC, MonsterType::GLASS };
    int poolSize = sizeof(randomPool) / sizeof(randomPool[0]);

    MonsterType selectedType;

    switch (stageChoice) {

    case 1:
        selectedType = randomPool[rand() % poolSize];
        break;

    case 2:
        selectedType = MonsterType::ALUMINUM;
        break;

    case 3:
        selectedType = MonsterType::IRON;
        break;

    default:
        std::cout << "잘못된 선택입니다. 1번(분리수거장)으로 이동합니다." << std::endl;
        selectedType = randomPool[rand() % poolSize];
    }


    return CreateMonster(selectedType, playerLevel);
}


void EnterBattle(Player* player, std::vector < std::pair<Item, int >> &items) {
    Monster* monster = StageMonster(player->GetLevel());

    StartBattle(player, monster, items);

    delete monster;
}

void StartBattle(Player* player, Monster* monster, std::vector<std::pair<Item, int>>& items) {

    std::cout << "\n" << player->GetName() << " VS " << monster->getName() << "\n" << std::endl;

    while (player->GetHp() > 0 && monster->getHp() > 0) {

        std::cout << "\n========플레이어 턴========\n" << std::endl;

        for (int i = 0; i < items.size(); i++) {
            std::cout << i + 1 << ". " << items[i].first.GetName() << " x" << items[i].second << std::endl;
        }
        std::cout << "사용할 아이템 번호 선택: ";
        int choice;
        std::cin >> choice;
        int index = choice - 1;
        if (index < 0 || index >= (int)items.size()) {
            std::cout << "잘못된 선택입니다." << std::endl;
            continue;
        }

        Item selectedItem = items[index].first;
        std::cout << "\n" << selectedItem.GetAttackText() << std::endl;

        if (selectedItem.GetCategory() == ItemCategory::WEAPON) {
            double damage = selectedItem.GetBaseATK();
            MonsterType monsterType = monster->getProperty();
            std::vector<MonsterType> strong = selectedItem.GetStrongAgainst();
            std::vector<MonsterType> weak = selectedItem.GetWeakAgainst();

            if (std::find(strong.begin(), strong.end(), monsterType) != strong.end()) {
                damage = damage * 1.5;
                std::cout << "효과가 굉장했다!" << std::endl;
            }
            else if (std::find(weak.begin(), weak.end(), monsterType) != weak.end()) {
                damage = damage / 1.5;
                std::cout << "효과가 별로다..." << std::endl;
            }
            else {
                std::cout << "효과는 보통이다." << std::endl;
            }

            int finalDamage = (int)damage;
            monster->setHp(monster->getHp() - finalDamage);
            std::cout << finalDamage << " 피해!" << std::endl;
            std::cout << monster->getName() << " 체력: " << monster->getHp() << std::endl;
        }
        else {
            if (selectedItem.GetHealHP() > 0) {
                player->SetHp(player->GetHp() + selectedItem.GetHealHP());
                std::cout << "체력 " << selectedItem.GetHealHP() << " 회복!" << std::endl;
            }
            if (selectedItem.GetBuffATK() > 0) {
                player->SetAttack(player->GetAttack() + selectedItem.GetBuffATK());
                std::cout << "공격력 " << selectedItem.GetBuffATK() << " 증가!" << std::endl;
            }
            if (selectedItem.GetBuffMaxHP() > 0) {
                player->SetMaxHp(player->GetMaxHp() + selectedItem.GetBuffMaxHP());
                std::cout << "최대 체력 " << selectedItem.GetBuffMaxHP() << " 증가!" << std::endl;
            }
            if (selectedItem.GetGainLevel() > 0) {
                player->SetLevel(player->GetLevel() + selectedItem.GetGainLevel());
                std::cout << "레벨 " << selectedItem.GetGainLevel() << " 업!" << std::endl;
            }
        }

        items[index].second -= 1;
        if (items[index].second <= 0) {
            items.erase(items.begin() + index);
        }

        if (monster->getHp() <= 0) {
            break;
        }

        monster->Attack(player);
        std::cout << "\n" << monster->getName() << "의 공격!" << std::endl;
        std::cout << player->GetName() << " 체력: " << player->GetHp() << std::endl;
    }

    std::cout << "\n===============" << std::endl;
    if (player->GetHp() <= 0) {
        std::cout << "패배..." << std::endl;
    }
    else if (monster->getHp() <= 0) {
        std::cout << monster->getName() << " 처치! 승리" << std::endl;

        int gainedExp = monster->getExpReward();
        player->GainExp(gainedExp);
        std::cout << "exp + " << gainedExp << " (" << player->GetExp() << "/100)" << std::endl;

        int gainedGold = monster->getGoldReward();
        player->AddGold(gainedGold);
        std::cout << "gold + " << gainedGold << " (보유골드 " << player->GetGold() << "g)" << std::endl;

        int dropChance = rand() % 100;
        if (dropChance < 30) {
            std::cout << "아이템 드롭 판정... 성공 (30%)" << std::endl;
        }
        else {
            std::cout << "아이템 드롭 판정... 실패 (30%)" << std::endl;
        }
    }
}
