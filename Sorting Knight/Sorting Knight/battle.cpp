#include "battle.h"
#include "Player.h"
#include "monster.h"
#include "Item.h"
#include "ItemManager.h"
#include "ui.h"
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

static bool stage2Entered = false;
static bool stage3Entered = false;

// 처치 집계 (몬스터 이름 -> 처치 수)
static std::map<std::string, int> monsterkillCount;
static int totalWinCount = 0;

// 전투 로그. 최근 4줄만 유지
static std::vector<std::string> battleLog;

static void AddLog(const std::string& msg) {
    battleLog.push_back(msg);
    while (battleLog.size() > 4) {
        battleLog.erase(battleLog.begin());
    }
}

Monster* StageMonster(int playerLevel, int& selectedStage) {

    MonsterType randomPool[] = { MonsterType::NONE, MonsterType::PAPER, MonsterType::PLASTIC, MonsterType::GLASS };
    int poolSize = sizeof(randomPool) / sizeof(randomPool[0]);

    std::string notice = "";

    while (true) {
        std::vector<std::string> body = {
            "",
            "  1. 쓰레기장",
            Color("  2. 분리수거장      (권장 레벨 5)",  playerLevel >= 5 ? "37" : "90"),
            Color("  3. 폐기처리장      (권장 레벨 10)", playerLevel >= 10 ? "37" : "90"),
            "",
            Color("  ※ 권장 레벨 미달 구역은 출입이 제한됩니다.", "90"),
            ""
        };
        std::vector<std::string> footer = {
            notice.empty() ? "" : Color(notice, "91"),
            "",
            "입장 구역: "
        };
        DrawScreen("구역 선택", body, footer);
        std::cout << "\033[37;14H";        

        int stageChoice;
        std::cin >> stageChoice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            notice = "잘못된 입력입니다.";
            continue;
        }

        MonsterType selectedType;

        switch (stageChoice) {

        case 1:
            selectedType = randomPool[rand() % poolSize];
            selectedStage = 1;
            return CreateMonster(selectedType, playerLevel);

        case 2:
            if (playerLevel < 5) {
                notice = "아직 분리수거할 수 없는 구역입니다.";
                continue;
            }
            if (stage2Entered) {
                notice = "이미 분리수거가 완료된 구역입니다.";
                continue;
            }
            selectedType = MonsterType::ALUMINUM;
            selectedStage = 2;
            return CreateMonster(selectedType, playerLevel);

        case 3:
            if (playerLevel < 10) {
                notice = "아직 분리수거할 수 없는 구역입니다.";
                continue;
            }
            if (stage3Entered) {
                notice = "이미 분리수거가 완료된 구역입니다.";
                continue;
            }
            selectedType = MonsterType::IRON;
            selectedStage = 3;
            return CreateMonster(selectedType, playerLevel);

        default:
            notice = "잘못된 선택입니다.";
            continue;
        }
    }
}


void EnterBattle(Player* player) {
    int selectedStage = 0;
    Monster* monster = StageMonster(player->GetLevel(), selectedStage);

    bool isWin = StartBattle(player, monster);
    if (isWin) {
        ClearStage(selectedStage);
        if (selectedStage == 3) {
            EnterEnding(player);
        }
    }
    delete monster;
}


bool StartBattle(Player* player, Monster* monster) {

    battleLog.clear();
    AddLog("> " + monster->getName() + " 이(가) 나타났다!");

    while (player->GetHp() > 0 && monster->getHp() > 0) {

        const std::vector<std::pair<Item, int>>& inventoryItems = player->GetInventory().GetItems();

        std::vector<std::string> body = {
            "",
            Color(monster->getName(), "91"),
            "HP  " + std::to_string(monster->getHp()),
            "",
            Color(player->GetName() + "   Lv." + std::to_string(player->GetLevel()), "92"),
            "HP  " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 20), "92")
                   + "   " + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()),
            "ATK " + std::to_string(player->GetAttack()),
            "",
            Color("──  사용할 아이템  ──", "90"),
            ""
        };  

        for (size_t i = 0; i < inventoryItems.size(); i++) {
            std::string line = "  " + std::to_string(i + 1) + ". " + inventoryItems[i].first.GetName() + " [" + inventoryItems[i].first.GetRarityString() + "등급]";
            if (inventoryItems[i].first.GetCategory() == ItemCategory::CONSUMABLE) {
                line += "  x" + std::to_string(inventoryItems[i].second);
            }
            body.push_back(line);
        }
        body.push_back("");

        std::vector<std::string> footer = battleLog;
        footer.push_back("");
        footer.push_back("아이템 번호: ");

        DrawScreen("전투 - " + monster->getName(), body, footer);
        std::cout << "\033[37;16H";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            AddLog(Color("> 잘못된 입력입니다.", "91"));
            continue;
        }

        int index = choice - 1;
        if (index < 0 || index >= (int)inventoryItems.size()) {
            AddLog(Color("> 잘못된 선택입니다.", "91"));
            continue;
        }

        Item selectedItem = inventoryItems[index].first;
        AddLog("> " + selectedItem.GetAttackText());

        if (selectedItem.GetCategory() == ItemCategory::WEAPON) {
            double damage = selectedItem.GetBaseATK();
            MonsterType monsterType = monster->getProperty();
            std::vector<MonsterType> strong = selectedItem.GetStrongAgainst();
            std::vector<MonsterType> weak = selectedItem.GetWeakAgainst();

            if (std::find(strong.begin(), strong.end(), monsterType) != strong.end()) {
                damage = damage * 1.5;
                AddLog(Color("> 효과가 굉장했다!", "96"));
            }
            else if (std::find(weak.begin(), weak.end(), monsterType) != weak.end()) {
                damage = damage / 1.5;
                AddLog(Color("> 효과가 별로다...", "90"));
            }
            else {
                AddLog("> 효과는 보통이다.");
            }

            int finalDamage = (int)damage;
            monster->setHp(monster->getHp() - finalDamage);
            AddLog(Color("> " + std::to_string(finalDamage) + " 피해!", "93"));
        }
        else {
            if (selectedItem.GetHealHP() > 0) {
                player->SetHp(player->GetHp() + selectedItem.GetHealHP());
                AddLog(Color("> 체력 " + std::to_string(selectedItem.GetHealHP()) + " 회복!", "92"));
            }
            if (selectedItem.GetBuffATK() > 0) {
                player->SetAttack(player->GetAttack() + selectedItem.GetBuffATK());
                AddLog(Color("> 공격력 " + std::to_string(selectedItem.GetBuffATK()) + " 증가!", "92"));
            }
            if (selectedItem.GetBuffMaxHP() > 0) {
                player->SetMaxHp(player->GetMaxHp() + selectedItem.GetBuffMaxHP());
                AddLog(Color("> 최대 체력 " + std::to_string(selectedItem.GetBuffMaxHP()) + " 증가!", "92"));
            }
            if (selectedItem.GetGainLevel() > 0) {
                player->SetLevel(player->GetLevel() + selectedItem.GetGainLevel());
                AddLog(Color("> 레벨 " + std::to_string(selectedItem.GetGainLevel()) + " 업!", "93"));
            }
        }

        if (selectedItem.GetCategory() == ItemCategory::CONSUMABLE) {
            player->GetInventory().RemoveItem(selectedItem.GetName(), 1);
        }

        if (monster->getHp() <= 0) {
            break;
        }

        monster->Attack(player);
        AddLog(Color("> " + monster->getName() + "의 공격!", "91"));
        AddLog("> " + player->GetName() + " 체력 " + std::to_string(player->GetHp()));
    }

    // ── 패배 ──
    if (player->GetHp() <= 0) {
        std::vector<std::string> body = {
            "", "",
            Color("        근무 중 순직하셨습니다.", "91"),
            "",
            Color("        규정 제17조에 의거, 후임은 배정되지 않습니다.", "90"),
            "", ""
        };
        std::vector<std::string> footer = { "[ Enter: 계속 ]" };
        DrawScreen("게임 오버", body, footer);
        std::cin.ignore();
        std::cin.get();
        return false;
    }

    // ── 승리 ──
    if (monster->getHp() <= 0) {
        int gainedExp = monster->getExpReward();
        player->GainExp(gainedExp);

        int gainedGold = monster->getGoldReward();
        player->AddGold(gainedGold);

        std::string dropText = "";
        int dropChance = rand() % 100;
        if (dropChance < 30) {
            static ItemManager itemManager;

            ItemRarity droppedRarity;
            int rarityRoll = rand() % 100;
            if (rarityRoll < 80) { droppedRarity = ItemRarity::C; }
            else { droppedRarity = ItemRarity::B; }

            Item droppedItem = itemManager.GetRandomItemByRarity(droppedRarity);
            player->GetInventory().AddItem(droppedItem, 1);
            dropText = droppedItem.GetName();
        }

        // 처치 집계
        monsterkillCount[monster->getName()]++;
        totalWinCount++;

        std::vector<std::string> body = {
            "",
            Color("  " + monster->getName() + " 처리 완료.", "93"),
            "",
            "  EXP   +" + std::to_string(gainedExp)
                        + "     (" + std::to_string(player->GetExp()) + "/100)",
            "  Gold  +" + std::to_string(gainedGold)
                        + "     (보유 " + std::to_string(player->GetGold()) + "G)",
            ""
        };
        if (!dropText.empty()) {
            body.push_back(Color("  획득   " + dropText, "96"));
        }
        else {
            body.push_back(Color("  회수 가능한 폐기물이 없습니다.", "90"));
        }
        body.push_back("");
        body.push_back(Color("  누적 처리 실적  " + std::to_string(totalWinCount) + "건", "90"));
        body.push_back("");

        std::vector<std::string> footer = {
            Color("규정 제4조에 의거, 처리 완료.", "90"),
            "",
            "[ Enter: 계속 ]"
        };
        DrawScreen("처리 완료", body, footer);
        std::cin.ignore();
        std::cin.get();
        return true;
    }

    return false;
}


void ClearStage(int stage) {
    if (stage == 2) stage2Entered = true;
    else if (stage == 3) stage3Entered = true;
}


// ── 엔딩 : 최종 민원 처리 대장 ────────────────────────────
void EnterEnding(Player* player) {

    std::vector<std::string> body = {
        "",
        Color("  - 최종 민원 처리 대장 -", "93"),
        "",
        "  담당자   " + player->GetName()
                     + " (" + player->GetJobName()
                     + ", Lv." + std::to_string(player->GetLevel()) + ")",
        "",
        Color("  ── 처리 내역 ──", "90"),
        ""
    };

    for (const auto& pair : monsterkillCount) {
        body.push_back("    " + pair.first + "   x" + std::to_string(pair.second));
    }

    body.push_back("");
    body.push_back(Color("  누적 처리 실적   " + std::to_string(totalWinCount) + "건", "96"));
    body.push_back("");
    body.push_back(Color("  폐기처리장 완전 정화 완료. 수고하셨습니다.", "92"));
    body.push_back("");

    std::vector<std::string> footer = {
        Color("※ 분리배출에 관한 법률은 여전히 폐지 상태입니다.", "90"),
        "",
        "[ Enter: 계속 ]"
    };

    DrawScreen("엔딩 - 업무 보고", body, footer);
    std::cin.ignore();
    std::cin.get();
}