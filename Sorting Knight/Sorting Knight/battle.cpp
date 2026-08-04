#define NOMINMAX
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
#include <windows.h>

static bool stage2Entered = false;
static bool stage3Entered = false;
static bool gameCleared = false;

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

bool IsGameCleared() {
    return gameCleared;
}

// ── 구역 진입 연출 (중간보스 / 최종보스) ──────────────────────
void ShowStageIntro(int stage, const std::string& monsterName) {
    std::vector<std::string> body(20, "");
    std::vector<std::string> art = {};
    std::string title;
    std::vector<std::string> footer;

    if (stage == 2) {
        title = "구역 진입 - 분리수거장";
        footer = { "[ Enter: 진입 ]" };
    }
    else {
        title = "구역 진입 - 폐기처리장";
        footer = { "[ Enter: 최후의 임무 ]" };
    }

    DrawScreen(title, body, art, footer);

    int startY = 5;
    int startX = 6;

    if (stage == 2) {
        TypeTextAt(startY + 2, startX, "분리수거장 안쪽 깊숙한 곳.", "90", 20);
        Sleep(200);

        TypeTextAt(startY + 4, startX, "찌그러진 캔들이 산처럼 쌓여 이상한 진동을 내고 있다.", "90", 15);
        Sleep(200);

        TypeTextAt(startY + 6, startX, "누군가... 아니, 무언가가 오랫동안 압축되지 못한 채", "90", 15);
        TypeTextAt(startY + 7, startX, "그 안에서 스스로 뭉쳐버린 듯하다.", "90", 15);
        Sleep(300);

        TypeTextAt(startY + 9, startX, "쿠구구구...", "37", 50);
        Sleep(400);

        TypeTextAt(startY + 11, startX, "[ " + monsterName + " ] 이(가) 그 형체를 드러냈다.", "91", 40);
        Sleep(250);
    }
    else if (stage == 3) {
        TypeTextAt(startY + 2, startX, "폐기처리장. 이 구역의 가장 깊은 곳.", "90", 20);
        Sleep(200);

        TypeTextAt(startY + 4, startX, "녹슨 철제 잔해들이 겹겹이 쌓여 거대한 그림자를 드리운다.", "90", 15);
        Sleep(200);

        TypeTextAt(startY + 6, startX, "30년간 아무도 손대지 못한 채 방치된 폐기물들이", "90", 15);
        TypeTextAt(startY + 7, startX, "하나의 의지를 가진 듯 꿈틀거리기 시작한다.", "90", 15);
        Sleep(300);

        TypeTextAt(startY + 9, startX, "이 곳을 정리하면, 진짜 마지막이다.", "37", 30);
        Sleep(400);

        TypeTextAt(startY + 11, startX, "[ " + monsterName + " ]", "93", 60);
        TypeTextAt(startY + 12, startX, "재활용의 역사가 남긴 최후의 잔재가 눈을 뜬다.", "96", 40);
        Sleep(300);
    }

    std::cout << "\033[37;18H";
    std::cin.ignore();
    std::cin.get();
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

        case 2: {
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
            Monster* boss2 = CreateMonster(selectedType, playerLevel);
            ShowStageIntro(2, boss2->getName());   // ★ 진입 연출
            return boss2;
        }

        case 3: {
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
            Monster* boss3 = CreateMonster(selectedType, playerLevel);
            ShowStageIntro(3, boss3->getName());   // ★ 진입 연출
            return boss3;
        }

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
            gameCleared = true;
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
            std::string item1 = std::to_string(idx1 + 1) + ". ";
            if (inventoryItems[idx1].first.GetCategory() == ItemCategory::WEAPON) {
                std::string rColor = (inventoryItems[idx1].first.GetRarity() == ItemRarity::S) ? "95" :
                    (inventoryItems[idx1].first.GetRarity() == ItemRarity::A) ? "94" :
                    (inventoryItems[idx1].first.GetRarity() == ItemRarity::B) ? "93" : "37";

                std::string nameAndRarity = inventoryItems[idx1].first.GetName() + " [" + inventoryItems[idx1].first.GetRarityString() + "등급]";
                // ★ 수정됨: 뒤에 붙어있던 속성(강/약) 텍스트를 지워서 공간 확보!
                item1 += Color(nameAndRarity, rColor.c_str());
            }
            else {
                item1 += inventoryItems[idx1].first.GetName() + " x" + std::to_string(inventoryItems[idx1].second) + " " + Color(inventoryItems[idx1].first.GetEffectString(), "92");
            }
            std::string line = "  " + item1;

            // 두 번째 열 
            int idx2 = r + rows;
            if (idx2 < itemCount) {
                int w1 = DisplayWidth(line);
                int pad = 44 - w1;
                if (pad < 2) pad = 2;

                std::string item2 = std::to_string(idx2 + 1) + ". ";
                if (inventoryItems[idx2].first.GetCategory() == ItemCategory::WEAPON) {
                    std::string rColor = (inventoryItems[idx2].first.GetRarity() == ItemRarity::S) ? "95" :
                        (inventoryItems[idx2].first.GetRarity() == ItemRarity::A) ? "94" :
                        (inventoryItems[idx2].first.GetRarity() == ItemRarity::B) ? "93" : "37";

                    std::string nameAndRarity = inventoryItems[idx2].first.GetName() + " [" + inventoryItems[idx2].first.GetRarityString() + "등급]";
                    // ★ 수정됨: 두 번째 열도 속성 텍스트 제거
                    item2 += Color(nameAndRarity, rColor.c_str());
                }
                else {
                    item2 += inventoryItems[idx2].first.GetName() + " x" + std::to_string(inventoryItems[idx2].second) + " " + Color(inventoryItems[idx2].first.GetEffectString(), "92");
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

            std::string effectText;
            std::string effectColor;

            if (std::find(strong.begin(), strong.end(), monsterType) != strong.end()) {
                damage = damage * 1.5;
                effectText = "효과가 굉장했다!";
                effectColor = "96";
            }
            else if (std::find(weak.begin(), weak.end(), monsterType) != weak.end()) {
                damage = damage / 1.5;
                effectText = "효과가 별로다...";
                effectColor = "90";
            }
            else {
                effectText = "효과는 보통이다.";
                effectColor = "93";
            }

            int finalDamage = (int)damage;
            monster->setHp(monster->getHp() - finalDamage);

            // ★ 상성 결과 + 데미지를 한 줄로 합침
            AddLog(Color("> " + effectText + " (" + std::to_string(finalDamage) + " 피해!)", effectColor.c_str()));
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
        std::vector<std::string> footer = { "[ Enter: 타이틀로 돌아가기 ]" };
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

    std::vector<std::string> body(20, "");
    std::vector<std::string> art = {};
    std::vector<std::string> footer = { "[ Enter: 계속 ]" };

    DrawScreen("엔딩 - 업무 보고", body, art, footer);

    int startY = 5;
    int startX = 6;

    TypeTextAt(startY + 1, startX, "폐기처리장의 마지막 잔재가 조용히 흩어진다.", "90", 30);
    Sleep(300);

    TypeTextAt(startY + 3, startX, "하지만 \"재공\"은 멈추지 않는다. 오늘도 분리수거를 위해 묵묵히 임무를 완수하며 복무한다.", "90", 25);
    Sleep(300);

    TypeTextAt(startY + 5, startX, "- 최종 민원 처리 대장 -", "93", 40);
    Sleep(400);

    std::string profile = "담당자   " + player->GetName()
        + " (" + player->GetJobName()
        + ", Lv." + std::to_string(player->GetLevel()) + ")";
    TypeTextAt(startY + 7, startX, profile, "37", 20);
    Sleep(300);

    TypeTextAt(startY + 9, startX, "── 처리 내역 ──", "90", 20);
    Sleep(200);

    int lineOffset = 11;
    for (const auto& pair : monsterkillCount) {
        std::string line = "  " + pair.first + "   x" + std::to_string(pair.second);
        TypeTextAt(startY + lineOffset, startX, line, "37", 15);
        lineOffset++;
    }
    Sleep(300);

    lineOffset++;
    TypeTextAt(startY + lineOffset, startX,
        "누적 처리 실적   " + std::to_string(totalWinCount) + "건", "96", 30);
    Sleep(500);

    lineOffset += 2;
    TypeTextAt(startY + lineOffset, startX,
        "폐기처리장 완전 정화 완료. 수고하셨습니다.", "92", 40);
    Sleep(600);

    std::cout << "\033[37;24H";
    std::cin.ignore();
    std::cin.get();
}

// 타이틀로 돌아가기 전에 전투 관련 스탯을 초기화하는 함수
void ResetBattleStats() {
    stage2Entered = false;
    stage3Entered = false;
    gameCleared = false;
    monsterkillCount.clear(); // 몬스터 처치 기록 맵 비우기
    totalWinCount = 0;        // 누적 처리 실적 0으로 리셋
    battleLog.clear();        // 이전 게임의 전투 로그도 비워주기
}