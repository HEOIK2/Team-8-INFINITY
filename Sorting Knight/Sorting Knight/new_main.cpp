#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <windows.h>

#include "main.h"
#include "type.h"
#include "gameText.h"
#include "Item.h"
#include "ItemManager.h"
#include "inventory.h"
#include "player.h"
#include "monster.h"
#include "battle.h"
#include "shop.h"
#include "ui.h"

// ── 입력 오류 방지 ────────────────────────────────────────
void ClearInputBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
}

// ── 프롤로그 ─────────────────────────────────────────────
void ShowIntro() {
    std::vector<std::string> body = {
        "", "",
        Color("분리배출에 관한 법률이 폐지되었다.", "37"),
        "",
        Color("사람들은 더 이상 쓰레기를 나누지 않았고,", "90"),
        Color("나눠지지 않은 것들은 스스로 형태를 갖추기 시작했다.", "90"),
        "",
        Color("재활용 기사단은 해체되었다. 예산이 없었다.", "90"),
        "",
        Color("남은 건 한 명. 마지막 공익근무요원.", "93"),
        "", ""
    };
    std::vector<std::string> art = {};
    std::vector<std::string> footer = { "[ Enter: 계속 ]" };
    DrawScreen("프롤로그", body, art, footer);
    std::cin.ignore();
    std::cin.get();
}

// ── 캐릭터 생성 ──────────────────────────────────────────
Player* InitializeGame() {
    std::string name;
    int cnt = 0;

    // 이름 입력
    while (true) {
        std::vector<std::string> body = {
            "", "당신의 이름을 입력하십시오.", "",
            Color("※ 근무일지에 기재될 성명입니다.", "90"), ""
        };
        std::vector<std::string> art = {};
        std::vector<std::string> footer = { "이름: " };
        DrawScreen("신규 요원 등록", body, art, footer);
        std::cout << "\033[37;11H";

        if (std::cin >> name) {
            ClearInputBuffer();
            if (!name.empty()) break;
        }
        else {
            ClearInputBuffer();
        }
    }

    // 직업 입력
    while (true) {
        std::vector<std::string> body = {
            "", name + " 요원, 배치 부서를 선택하십시오.", "",
            "  1. 일반쓰레기 수거반   — 대나무 빗자루 (C)",
            "  2. 도로교통 미화반     — 분리수거 집게 (C)",
            "  3. 분리배출 지원반     — 평범한 가위 (C)",
            "  4. 대형폐기물 철거반   — 작업용 쇠지렛대 (C)",
            ""
        };
        std::vector<std::string> footer = { "부서 번호(1-4): " };

        std::vector<std::string> art = {};
        DrawScreen("부서 배치", body, art, footer);
        std::cout << "\033[37;20H";

        if (std::cin >> cnt) {
            ClearInputBuffer();
            if (cnt >= 1 && cnt <= 4) break;
        }
        else {
            ClearInputBuffer();
        }
    }

    switch (cnt) {
    case 1: return new Player(name, Job::DeptGeneral);
    case 2: return new Player(name, Job::DeptStreet);
    case 3: return new Player(name, Job::DeptRecycle);
    case 4: return new Player(name, Job::DeptDemolition);
    default: return new Player(name, Job::DeptGeneral);
    }
}

// ── 직업별 초기 아이템 ────────────────────────────────────
void GiveInitialItems(Player* player, ItemManager& itemManager) {
    switch (player->GetJob()) {
    case Job::DeptGeneral:
        player->GetInventory().AddItem(itemManager.GetItem("대나무 빗자루"), 1);
        break;
    case Job::DeptStreet:
        player->GetInventory().AddItem(itemManager.GetItem("분리수거 집게"), 1);
        break;
    case Job::DeptRecycle:
        player->GetInventory().AddItem(itemManager.GetItem("평범한 가위"), 1);
        break;
    case Job::DeptDemolition: // 신설된 철거반에게 쇠지렛대 지급!
        player->GetInventory().AddItem(itemManager.GetItem("작업용 쇠지렛대"), 1);
        break;
    default:
        break;
    }
}

// ── 메인 메뉴 ────────────────────────────────────────────
int ShowMainMenu(Player* player) {
    std::vector<std::string> body = {
        "",
        player->GetName() + "   Lv." + std::to_string(player->GetLevel()),
        "HP  " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 20), "92") + "  "
               + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()),
        "ATK " + std::to_string(player->GetAttack())
               + "      Gold " + std::to_string(player->GetGold()) + "G",
        "",
        "  1. 전투",
        "  2. 상점",
        "  3. 인벤토리",
        "  4. 근무 기록부",
        Color("  9. ???", "90"),
        "  0. 타이틀로",
        ""
    };

    std::vector<std::string> art = {
     
    };

    std::vector<std::string> footer = {
        Color("규정 제4조에 의거, 근무를 계속하십시오.", "90"),
        "", "선택: "
    };

    DrawScreen("메인 메뉴", body, art, footer);
    std::cout << "\033[37;11H";
    int choice;
    std::cin >> choice;
    ClearInputBuffer();
    return choice;
}

// ── 등급 색상 반환 보조 함수 ────────────────────────────────
std::string GetRarityColor(ItemRarity r) {
    switch (r) {
    case ItemRarity::C: return "37"; // 흰색
    case ItemRarity::B: return "93"; // 밝은 파랑
    case ItemRarity::A: return "94"; // 밝은 보라(자홍)
    case ItemRarity::S: return "95"; // 밝은 노랑
    default: return "37";
    }
}

// ── 인벤토리 화면 (사용 기능 및 색상 추가) ────────────────────────────────────────
void ShowInventoryScreen(Player* player) {
    std::string notice = "";

    while (true) {
        const std::vector<std::pair<Item, int>>& items = player->GetInventory().GetItems();
        std::vector<std::string> body = { "" };

        if (items.empty()) {
            body.push_back(Color("  (비어 있음)", "90"));
        }
        else {
            int i = 1;
            for (const auto& slot : items) {
                const Item& item = slot.first;
                // ★ 수정됨: 이름은 여기서 더하지 않고 번호만 남깁니다.
                std::string line = "  " + std::to_string(i) + ". ";

                if (item.GetCategory() == ItemCategory::WEAPON) {
                    // ★ 수정됨: 이름과 [등급]을 한 덩어리로 묶어서 칠합니다!
                    std::string nameAndRarity = item.GetName() + " [" + item.GetRarityString() + "등급]";
                    line += Color(nameAndRarity, GetRarityColor(item.GetRarity()).c_str()) + " "
                        + Color(item.GetPropertyString(), "90") + "   "
                        + std::to_string(item.GetPrice()) + "G";
                }
                else {
                    line += item.GetName() + " x" + std::to_string(slot.second) + " "
                        + Color(item.GetEffectString(), "92") + "   "
                        + std::to_string(item.GetPrice()) + "G";
                }
                body.push_back(line);
                i++;
            }
        }
        body.push_back("");

        std::vector<std::string> art = {
        
            ""
        };

        std::vector<std::string> footer = {
            "소지 " + std::to_string(player->GetInventory().GetTotalItemCount())
                    + "/" + std::to_string(Inventory::MAX_ITEM_COUNT),
            notice,
            "번호 선택 (0: 돌아가기): "
        };

        DrawScreen("인벤토리", body, art, footer);
        std::cout << "\033[37;27H";
        notice = "";

        int choice;
        if (!(std::cin >> choice)) {
            ClearInputBuffer();
            notice = Color("[!] 잘못된 입력입니다.", "91");
            continue;
        }
        ClearInputBuffer();

        if (choice == 0) break;

        int index = choice - 1;
        if (index < 0 || index >= (int)items.size()) {
            notice = Color("[!] 존재하지 않는 번호입니다.", "91");
            continue;
        }

        const Item& selectedItem = items[index].first;
        if (selectedItem.GetCategory() == ItemCategory::CONSUMABLE) {
            UseResult res = player->GetInventory().UseItem(selectedItem.GetName(), *player);
            if (res == UseResult::SUCCESS) {
                notice = Color("[사용 완료] " + selectedItem.GetName() + "을(를) 사용했습니다.", "92");
            }
        }
        else {
            notice = Color("[!] 무기/장비는 여기서 사용할 수 없습니다.", "91");
        }
    }
}

// ── 근무 기록부(스탯) ────────────────────────────────────
void ShowStatusScreen(Player* player) {
    std::vector<std::string> body = {
        "",
        "  이름      " + player->GetName(),
        "  레벨      Lv." + std::to_string(player->GetLevel()),
        "",
        "  HP        " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 20), "92") + "  "
                       + std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()),
        "  EXP       " + Color(MakeGauge(player->GetExp(), 100, 20), "96") + "  "
                       + std::to_string(player->GetExp()) + "/100",
        "",
        "  공격력    " + std::to_string(player->GetAttack()),
        "  골드      " + std::to_string(player->GetGold()) + "G",
        "",
    };
    std::vector<std::string> art = {
           Color("  - 현재 민원 처리 대장 -", "93"),
        "",
         "  담당자   " + player->GetName()
                     + " (" + player->GetJobName()
                     + ", Lv." + std::to_string(player->GetLevel()) + ")",
        "",
            Color("  ── 처리 내역 ──", "90"),
        ""
    };

    for (const auto& pair : GetMonsterKillCount()) {
        art.push_back("    " + pair.first + "   x" + std::to_string(pair.second));
    }

    std::vector<std::string> footer = { "[ Enter: 돌아가기 ]" };
    DrawScreen("근무 기록부", body, art, footer);
    std::cin.ignore();
    std::cin.get();
}

// ── 개발자용 치트 ────────────────────────────────────────
void ShowDebugMenu(Player* player) {
    std::string notice = "";

    while (true) {
        std::vector<std::string> body = {
            "",
            Color("  ※ 관리자 전용 단말입니다.", "90"),
            "",
            "  1. 경험치 1000 지급",
            "  2. 10000G 지급",
            "  3. 최강 무기 획득",
            "  4. 나가기",
            ""
        };
        std::vector<std::string> art = {};
        std::vector<std::string> footer = {
            "Lv." + std::to_string(player->GetLevel())
            + "    EXP " + std::to_string(player->GetExp())
            + "    Gold " + std::to_string(player->GetGold()) + "G",
            notice,
            "선택: "
        };

        DrawScreen("특별 감사 단말", body, art, footer);
        std::cout << "\033[37;11H";
        notice = "";

        int choice;
        if (!(std::cin >> choice)) {
            ClearInputBuffer();
            notice = Color("[!] 잘못된 입력입니다.", "91");
            continue;
        }
        ClearInputBuffer();

        if (choice == 1) {
            player->GainExp(1000);
            notice = Color("[지급 완료] 경험치 1000", "92");
        }
        else if (choice == 2) {
            player->SetGold(player->GetGold() + 10000);
            notice = Color("[지급 완료] 10000G", "92");
        }
        else if (choice == 3) {
            player->GetInventory().AddItem(ItemManager().GetItem("소형 블랙홀 압축기"), 1);
            player->GetInventory().AddItem(ItemManager().GetItem("클린 월드 엔드 캐논"), 1);
            player->GetInventory().AddItem(ItemManager().GetItem("플라즈마 용융 토치"), 1);
            notice = Color("[지급 완료] 최강 무기 세트", "92");
        }
        else if (choice == 4) {
            break;
        }
        else {
            notice = Color("[!] 잘못된 선택입니다.", "91");
        }
    }
}

// ── main ────────────────────────────────────────────────
int main() {
    UI_Init();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    srand((unsigned int)time(nullptr));

    bool isProgramRunning = true;

    while (isProgramRunning) {
        ShowMainTitleArt();
        ShowIntro();

        ItemManager itemManager;
        Player* player = InitializeGame();
        if (!player) { continue; }

        GiveInitialItems(player, itemManager);
        InitShop(player, &itemManager);

        ResetBattleStats();

        bool inMainMenu = true;
        while (inMainMenu) {
            int mainChoice = ShowMainMenu(player);

            switch (mainChoice) {
            case 1:
                EnterBattle(player);
                if (player->GetHp() <= 0) {
                    inMainMenu = false;
                }
                break;
            case 2:
                EnterShopMenu();
                break;
            case 3:
                // ★ 인벤토리 함수에 맞게 player 객체 자체를 전달하도록 수정됨!
                ShowInventoryScreen(player);
                break;
            case 4:
                ShowStatusScreen(player);
                break;
            case 9:
                ShowDebugMenu(player);
                break;
            case 0:
                inMainMenu = false;
                break;
            default:
                break;
            }
        }

        delete player;
        player = nullptr;
    }
    return 0;
}