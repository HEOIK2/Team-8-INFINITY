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

// ── 타이틀 메뉴 ──────────────────────────────────────────
int ShowTitleMenu() {
    std::vector<std::string> body = {
        "", "",
        Color("        RECYCLING KNIGHT", "36"),
        Color("        재활용 공익근무요원 : 요원의 마지막 기록", "90"),
        "", "",
        "        1. 게임 시작",
        "        2. 게임 종료",
        ""
    };
    std::vector<std::string> art = {};
    std::vector<std::string> footer = {
        Color("폐지된 법을 집행하러 갈 시간입니다.", "90"),
        "", "선택: "
    };
    
    DrawScreen("타이틀", body, art, footer);
    std::cout << "\033[37;11H";
    int choice;
    std::cin >> choice;
    ClearInputBuffer();
    return choice;
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

        // (UI 분할선이 적용된 DrawScreen 호출, art가 필요하다면 빈 벡터를 추가하세요)
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
               + "     Gold " + std::to_string(player->GetGold()) + "G",
        "",
        "  1. 전투",
        "  2. 상점",
        "  3. 인벤토리",
        "  4. 근무 기록부",
        Color("  9. ???", "90"),
        "  0. 타이틀로",
        ""
    };

    // ★ 빈칸 아래쪽에 들어갈 아스키 아트
    std::vector<std::string> art = {
        "",
        Color(R"(         __ )", "90"),
        Color(R"(        /  \       [ 경고: 미분류 폐기물 감지 ] )", "93"),
        Color(R"(       /____\      오늘도 무사히 분리수거를 완료하십시오. )", "90"),
        Color(R"(       |    | )", "90"),
        Color(R"(       |____| )", "90"),
        ""
    };

    std::vector<std::string> footer = {
        Color("규정 제4조에 의거, 근무를 계속하십시오.", "90"),
        "", "선택: "
    };

    // ★ 순서 변경: title, body, art, footer
    DrawScreen("메인 메뉴", body, art, footer);
    std::cout << "\033[37;11H";
    int choice;
    std::cin >> choice;
    ClearInputBuffer();
    return choice;
}

// ── 인벤토리 화면 ────────────────────────────────────────
void ShowInventoryScreen(const Inventory& inv) {
    const std::vector<std::pair<Item, int>>& items = inv.GetItems();

    std::vector<std::string> body = { "" };
    if (items.empty()) {
        body.push_back(Color("  (비어 있음)", "90"));
    }
    else {
        int i = 1;
        for (const auto& slot : items) {
            const Item& item = slot.first;
            std::string line = "  " + std::to_string(i) + ". " + item.GetName();

            if (item.GetCategory() == ItemCategory::WEAPON) {
                // ★ 무기: 등급과 함께 속성 정보(강/약)를 표시합니다.
                line += " [" + item.GetRarityString() + "등급] "
                    + Color(item.GetPropertyString(), "90") + "   "
                    + std::to_string(item.GetPrice()) + "G";
            }
            else {
                // ★ 소비 아이템: N등급을 삭제하고 수량 옆에 효과를 표시합니다.
                line += " x" + std::to_string(slot.second) + " "
                    + Color(item.GetEffectString(), "92") + "   "
                    + std::to_string(item.GetPrice()) + "G";
            }
            body.push_back(line);
            i++;
        }
    }
    body.push_back("");
    std::vector<std::string> art = {};
    std::vector<std::string> footer = {
        "소지 " + std::to_string(inv.GetTotalItemCount())
                + "/" + std::to_string(Inventory::MAX_ITEM_COUNT),
        "", "[ Enter: 돌아가기 ]"
    };
    DrawScreen("인벤토리", body, art, footer);
    
    std::cin.ignore();
    std::cin.get();
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
            notice = Color("[지급 완료] 플라즈마 용융 토치", "92");
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
        // 1. 첫 화면 (아스키 아트) 출력. 이 함수 안에 std::cin.get()이 있어서 엔터를 기다립니다.
        ShowMainTitleArt();

        // 2. 불필요한 두 번째 타이틀 메뉴 제거 완료
        // 엔터를 누르면 바로 프롤로그로 직행!
        ShowIntro();

        ItemManager itemManager;
        Player* player = InitializeGame();
        if (!player) { continue; }

        GiveInitialItems(player, itemManager);
        InitShop(player, &itemManager);

        // 이전 회차의 킬 카운트 및 스테이지 잠금 상태 리셋
        ResetBattleStats();

        bool inMainMenu = true;
        while (inMainMenu) {
            int mainChoice = ShowMainMenu(player);

            switch (mainChoice) {
            case 1:
                EnterBattle(player);
                // 전투 후 체력이 0 이하라면 강제로 타이틀로(유령 요원 방지)
                if (player->GetHp() <= 0) {
                    inMainMenu = false;
                }
                break;
            case 2:
                EnterShopMenu();
                break;
            case 3:
                ShowInventoryScreen(player->GetInventory());
                break;
            case 4:
                ShowStatusScreen(player);
                break;
            case 9:
                ShowDebugMenu(player);
                break;
            case 0:
                inMainMenu = false; // 0번 누르면 다시 첫 아스키 아트 화면으로 돌아감
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