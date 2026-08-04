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

// 몬스터 킬 카운트 반환
std::map<std::string, int> GetMonsterKillCount() {
	return monsterkillCount;
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
        std::vector<std::string> art = {};
        std::vector<std::string> footer = {
            notice.empty() ? "" : Color(notice, "91"),
            "",
            "입장 구역: "
        };
        DrawScreen("구역 선택", body, art, footer);
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

        // 1. 왼쪽 영역 (스탯) 데이터 구축
        // 1. 왼쪽 영역 (스탯) 데이터 구축 - 텍스트 길이 압축
        std::vector<std::string> leftPane = {
            "",
            Color(monster->getName(), "91"),
            // HP 게이지 칸 수를 15 -> 10칸으로 줄여 공간 확보
            "HP " + Color(MakeGauge(monster->getHp(), monster->getMaxHp(), 10), "91") + " "
                   + std::to_string(monster->getHp()) + "/" + std::to_string(monster->getMaxHp()),
            "ATK " + std::to_string(monster->getAtk()),
            "",
            Color(player->GetName() + " Lv." + std::to_string(player->GetLevel()), "92"),
            "HP " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 10), "92") + " "
                   + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()),
            "ATK " + std::to_string(player->GetAttack())
        };

        // 2. 오른쪽 영역 (아이템 목록)
        std::vector<std::string> rightPane = {
            "",
            Color("──  사용할 아이템  ──", "90")
        };

        int itemCount = inventoryItems.size();
        int rows = itemCount;

        if (itemCount > 6) {
            if (itemCount <= 12) rows = 6;
            else rows = (itemCount + 1) / 2;
        }

        for (int r = 0; r < rows; ++r) {
            // 첫 번째 열
            int idx1 = r;
            std::string item1 = std::to_string(idx1 + 1) + ". " + inventoryItems[idx1].first.GetName();
            if (inventoryItems[idx1].first.GetCategory() == ItemCategory::WEAPON) {
                // 무기 속성 추가
                item1 += " [" + inventoryItems[idx1].first.GetRarityString() + "등급] " + Color(inventoryItems[idx1].first.GetPropertyString(), "90");
            }
            else {
                // 소비템 [등급] 삭제 및 효과 추가
                item1 += " x" + std::to_string(inventoryItems[idx1].second) + " " + Color(inventoryItems[idx1].first.GetEffectString(), "92");
            }
            std::string line = "  " + item1;

            // 두 번째 열 
            int idx2 = r + rows;
            if (idx2 < itemCount) {
                int w1 = DisplayWidth(line);
                // 1열의 고정 너비를 44칸으로 넉넉하게 확장 (속성 텍스트 공간 확보)
                int pad = 44 - w1;
                if (pad < 2) pad = 2;

                std::string item2 = std::to_string(idx2 + 1) + ". " + inventoryItems[idx2].first.GetName();
                if (inventoryItems[idx2].first.GetCategory() == ItemCategory::WEAPON) {
                    item2 += " [" + inventoryItems[idx2].first.GetRarityString() + "등급] " + Color(inventoryItems[idx2].first.GetPropertyString(), "90");
                }
                else {
                    item2 += " x" + std::to_string(inventoryItems[idx2].second) + " " + Color(inventoryItems[idx2].first.GetEffectString(), "92");
                }
                line += std::string(pad, ' ') + item2;
            }
            rightPane.push_back(line);
        }

        // 3. 좌우 데이터를 합쳐서 최종 body 완성
        std::vector<std::string> body;
        size_t maxLines = std::max(leftPane.size(), rightPane.size());

        for (size_t i = 0; i < maxLines; ++i) {
            std::string l = (i < leftPane.size()) ? leftPane[i] : "";
            std::string r = (i < rightPane.size()) ? rightPane[i] : "";

            int currentWidth = DisplayWidth(l);

            // ★ 왼쪽 창을 28칸으로 훅 당겨서 압축! 속성 텍스트가 들어갈 넉넉한 공간 확보
            int pad1 = 28 - currentWidth;
            if (pad1 < 1) pad1 = 1;

            std::string separator = Color("│", "90");
            int pad2 = 2; // 간격 최소화

            body.push_back("  " + l + std::string(pad1, ' ') + separator + std::string(pad2, ' ') + r);
        }
        body.push_back("");

        // ★ 부활한 art 영역! (나중에 몬스터 타입에 따라 아트를 다르게 바꿀 수도 있습니다)
        std::vector<std::string> art = {
           
        };

        // 푸터 영역 (로그 출력)
        std::vector<std::string> footer = battleLog;
        footer.push_back("");
        footer.push_back("아이템 번호: ");

        // ★ 4개짜리 DrawScreen 호출로 변경! (body, art, footer 순서)
        DrawScreen("전투 - " + monster->getName(), body, art, footer);

        // ★ 입력 커서 위치 조정 (아트가 들어가면서 줄이 늘어났으므로 15H -> 22H 쯤으로 내려줍니다)
        std::cout << "\033[37;22H";

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
            double damage = player->GetAttack() + selectedItem.GetBaseATK();
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
        std::vector<std::string> art = {};
        std::vector<std::string> footer = { "[ Enter: 계속 ]" };
        DrawScreen("게임 오버", body, art, footer);
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

        player->SetHp(player->GetMaxHp());

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
        std::vector<std::string> art = {};
        std::vector<std::string> footer = {
            Color("규정 제4조에 의거, 처리 완료.", "90"),
            "",
            "[ Enter: 계속 ]"
        };
        DrawScreen("처리 완료", body, art, footer);
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

    std::vector<std::string> art = {};
    DrawScreen("엔딩 - 업무 보고", body, art, footer);
    std::cin.ignore();
    std::cin.get();
}

// 타이틀로 돌아가기 전에 전투 관련 스탯을 초기화하는 함수
void ResetBattleStats() {
    stage2Entered = false;
    stage3Entered = false;
    monsterkillCount.clear(); // 몬스터 처치 기록 맵 비우기
    totalWinCount = 0;        // 누적 처리 실적 0으로 리셋
    battleLog.clear();        // 이전 게임의 전투 로그도 비워주기
}