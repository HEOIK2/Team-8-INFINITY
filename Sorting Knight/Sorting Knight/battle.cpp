#pragma once
#include "battle.h"
#include "Player.h"
#include "monster.h"
#include "Item.h"
#include "main.h"

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

        if (index < 0 || index >= items.size()) {
            std::cout << "잘못된 입력입니다." << std::endl;
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
                std::cout << "효과가 보통이다." << std::endl;
            }

            int finalDamage = (int)damage;
            monster->setHp(monster->getHp() - finalDamage);

            std::cout << finalDamage << " 데미지!" << std::endl;
            std::cout << monster->getName() << " 체력:" << monster->getHp() << std::endl;
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
        std::cout << "exp + " << gainedExp << " (" << player->GetExp() << " / " << player->GetExpPerLevel() << ")" << std::endl;

        int gainedGold = monster->getGoldReward();
        player->SetGold(player->GetGold() + gainedGold);
        std::cout << "gold + " << gainedGold << " (현재 골드 " << player->GetGold() << "g)" << std::endl;

        int dropChance = rand() % 100;
        if (dropChance < 30) {
            std::cout << "아이템 드랍 판정... 성공!! (30%)" << std::endl;
        }
        else {
            std::cout << "아이템 드랍 판정... 실패..(30%)" << std::endl;
        }

    }
}