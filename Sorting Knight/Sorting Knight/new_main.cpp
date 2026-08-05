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

// ── 타이핑 효과 보조 함수 (좌표 지정 기능 추가) ──
void TypeTextAt(int row, int col, const std::string& text, const std::string& colorCode, int delay) {
    // 지정된 좌표로 커서 이동 후 색상 적용
    std::cout << "\033[" << row << ";" << col << "H";
    std::cout << "\033[" << colorCode << "m";

    for (size_t i = 0; i < text.length(); ++i) {
        std::cout << text[i];

        // UTF-8 인코딩에서 한글이 깨지지 않도록 바이트 계산
        bool isLastByte = (i + 1 == text.length()) || ((text[i + 1] & 0xC0) != 0x80);
        if (isLastByte) {
            std::cout << std::flush;
            // 쉼표나 마침표에서는 타자기처럼 잠시 쉬어갑니다.
            if (text[i] == '.' || text[i] == ',') Sleep(delay * 4);
            else Sleep(delay);
        }
    }
    std::cout << "\033[0m"; // 색상 초기화
}

// ── 프롤로그 (UI 테두리 유지 + 시네마틱 타자 연출) ─────────────────────────
void ShowIntro() {
    std::vector<std::string> body = {
        "", "",
        "                                                              ", // 20XX년
        "",
        "                                                              ", // 30년
        "                                                              ",
        "                                                              ",
        "",
        "                                                              ", // 법률 폐지
        "                                                              ",
        "                                                              ",
        "",
        "                                                              ", // 소집해제
        "                                                              ",
        "",
        "                                                              ", // 묵묵히
        "",
        "                                                              ", // 최후의 1인
        "                                                              ",
        "", ""
    };
    std::vector<std::string> art = {};
    std::vector<std::string> footer = { "[ Enter: 계속 ]" };

    DrawScreen(Color("프롤로그", "93"), body, art, footer);

    int startY = 5;
    int startX = 6;

    TypeTextAt(startY + 2, startX, "20XX년.", "93", 50);
    Sleep(250);

    TypeTextAt(startY + 4, startX, "재활용 공익근무요원 제도가 도입된 지 어언 30년.", "90", 15);
    Sleep(150);

    TypeTextAt(startY + 5, startX, "10년이면 강산도 변한다는 말이 있지.", "90", 15);
    TypeTextAt(startY + 6, startX, "30년이 지난 지금, 많은 것들이 바뀌었다.", "90", 15);
    Sleep(150);

    TypeTextAt(startY + 8, startX, "??? : [생활폐기물 분리배출 등에 관한 법률]이 폐지되었음을 알린다.", "37", 20);
    TypeTextAt(startY + 9, startX, "그날부터 수많은 폐기물들이 분리수거되지 못한채 버려졌고 , ", "90", 15);
    TypeTextAt(startY + 10, startX, "그렇게 버려진 폐기물들은 소멸되지 못한채 자아를 가지기 시작한다.", "90", 15);
    Sleep(250);

    TypeTextAt(startY + 12, startX, "분리수거에 신물이 난 요원들은 하나둘 소집해제를 하기 시작했고,", "90", 15);
    TypeTextAt(startY + 13, startX, "머지않아 재활용센터엔 아무도 남지 않게 되었다.....", "90", 20);
    Sleep(400); // 극적인 정적은 좀 남겨둠

    TypeTextAt(startY + 15, startX, "라고 모두가 알고 있을 때, 묵묵히 사회 복무를 이어가는 한 남자가 있었다.", "37", 20);
    Sleep(300);

    TypeTextAt(startY + 17, startX, "폐지된 법을 여전히 집행하는 마지막이자 유일한 재활용 공익근무요원.", "96", 30);
    Sleep(250);
    TypeTextAt(startY + 18, startX, "일명 \"재 공\"이라 불린 남자의 이야기다.", "93", 40);

    std::cout << "\033[37;15H";
    std::cin.ignore();
    std::cin.get();
}

// ── 표시 폭 기준 가운데 정렬 (한글 2칸, 색상 코드 무시) ──
static std::string PadCenter(const std::string& s, int width) {
    int w = DisplayWidth(s);
    int left = (width - w) / 2;
    if (left < 0) { left = 0; }
    int right = width - w - left;
    if (right < 0) { right = 0; }
    return std::string(left, ' ') + s + std::string(right, ' ');
}

// ── 요원 프로필 아트: 출입증 명찰 ─────────────────────────
// name이 비어 있으면 빈 명찰, 이름이 있으면 명찰 안에 이름을 새겨준다.
static std::vector<std::string> GetNameTagArt(const std::string& name) {
    // 출입증 (내부 폭 30) + 이름 칸 (내부 폭 22)
    const int cardW = 30;
    const int tagW = 22;
    auto cardLine = [&](const std::string& inner) {
        return "|" + PadCenter(inner, cardW) + "|";
        };

    std::string tagTop = "+" + std::string(tagW, '-') + "+";
    std::string tagMid = "|" + PadCenter(name.empty() ? "" : Color(name, "96"), tagW) + "|";
    std::string tagBot = "+" + std::string(tagW, '-') + "+";

    std::vector<std::string> card = {
        PadCenter("____", cardW + 2),                    // 목걸이 끈 고리
        PadCenter("|  |", cardW + 2),
        "." + std::string(cardW, '=') + ".",
        cardLine(Color("재활용센터 출입증", "93")),
        "|" + std::string(cardW, '-') + "|",
        cardLine(""),
        cardLine(tagTop),
        cardLine(tagMid),
        cardLine(tagBot),
        cardLine(Color("SORTING  KNIGHT", "90")),
        cardLine(""),
        "'" + std::string(cardW, '=') + "'",
    };

    // 화면 중앙 배치
    const int leftPad = (114 - (cardW + 2)) / 2;
    std::vector<std::string> result;
    for (const auto& line : card) {
        result.push_back(std::string(leftPad, ' ') + line);
    }
    return result;
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
        std::vector<std::string> art = GetNameTagArt("");  // 빈 명찰
        std::vector<std::string> footer = { "이름: " };
        DrawScreen(Color("요원 프로필 등록", "96"), body, art, footer);
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
            "", name + " 요원, 근무 중인 부서를 기입하십시오.", "",
            "  " + Color("1", "96") + ". " + Color("일반쓰레기 수거반", "97") + "   — " + Color("대나무 빗자루 (C)", "93"),
            "  " + Color("2", "96") + ". " + Color("도로교통 미화반  ", "97") + "   — " + Color("분리수거 집게 (C)", "93"),
            "  " + Color("3", "96") + ". " + Color("분리배출 지원반  ", "97") + "   — " + Color("평범한 가위 (C)", "93"),
            "  " + Color("4", "96") + ". " + Color("대형폐기물 철거반", "97") + "   — " + Color("작업용 쇠지렛대 (C)", "93"),
            ""
        };
        std::vector<std::string> footer = { "부서 번호(1-4): " };

        std::vector<std::string> art = GetNameTagArt(name);  // 명찰에 입력받은 이름 표시
        DrawScreen(Color("부서 기입", "96"), body, art, footer);
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
        Color(player->GetName(), "97") + "   " + Color("Lv." + std::to_string(player->GetLevel()), "96"),
        "HP  " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 20), "92") + "  "
               + Color(std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()), "92"),
        "ATK " + Color(std::to_string(player->GetAttack()), "91")
               + "      Gold " + Color(std::to_string(player->GetGold()) + "G", "93"),
        "",
        "  " + Color("1", "96") + ". 전투",
        "  " + Color("2", "96") + ". 상점",
        "  " + Color("3", "96") + ". 인벤토리",
        "  " + Color("4", "96") + ". 근무 기록부",
        Color("  9. ???", "90"),
        Color("  0. 게임 종료", "90"),
        ""
    };

    std::vector<std::string> art = {
     
    };

    std::vector<std::string> footer = {
        Color("규정 제4조에 의거, 근무를 계속하십시오.", "90"),
        "", "선택: "
    };

    DrawScreen(Color("메인 메뉴", "96"), body, art, footer);
    std::cout << "\033[37;11H";
    int choice;
    if (!(std::cin >> choice)) {
        if (std::cin.eof()) { return 0; }  // 입력 스트림이 닫히면 안전하게 종료
        ClearInputBuffer();
        return -1;  // 문자 등 잘못된 입력 → 메뉴 다시 표시
    }
    ClearInputBuffer();
    return choice;
}

// ── 등급 색상 반환 보조 함수 ────────────────────────────────
std::string GetRarityColor(ItemRarity r) {
    switch (r) {
    case ItemRarity::C: return "37"; // 흰색 (일반)
    case ItemRarity::B: return "94"; // 밝은 파랑 (레어)
    case ItemRarity::A: return "95"; // 밝은 보라 (에픽)
    case ItemRarity::S: return "93"; // 금색 (전설)
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
                        + Color(std::to_string(item.GetPrice()) + "G", "93");
                }
                else {
                    line += item.GetName() + " x" + std::to_string(slot.second) + " "
                        + Color(item.GetEffectString(), "92") + "   "
                        + Color(std::to_string(item.GetPrice()) + "G", "93");
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

        DrawScreen(Color("인벤토리", "96"), body, art, footer);
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
        "  이름      " + Color(player->GetName(), "97"),
        "  레벨      " + Color("Lv." + std::to_string(player->GetLevel()), "96"),
        "",
        "  HP        " + Color(MakeGauge(player->GetHp(), player->GetMaxHp(), 20), "92") + "  "
                       + Color(std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()), "92"),
        "  EXP       " + Color(MakeGauge(player->GetExp(), 100, 20), "96") + "  "
                       + Color(std::to_string(player->GetExp()) + "/100", "96"),
        "",
        "  공격력    " + Color(std::to_string(player->GetAttack()), "91"),
        "  골드      " + Color(std::to_string(player->GetGold()) + "G", "93"),
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
    DrawScreen(Color("근무 기록부", "92"), body, art, footer);
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
            "  3. 등급별 무기 세트 획득 (C~S, 11종)",
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

        DrawScreen(Color("특별 감사 단말", "90"), body, art, footer);
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
            // 시연용: C~S 전 등급 무기 세트 (총 11개, 12개 미만 유지)
            ItemManager cheatItems;
            const std::vector<std::string> cheatWeapons = {
                // C등급 x2
                "두꺼운 고무장갑", "평범한 가위",
                // B등급 x3
                "네오디뮴 자석 석궁", "고압 세척건", "신문지 해머",
                // A등급 x3
                "초음파 유리 분쇄기", "고온 유기물 분해기", "레이저 라벨 절단포",
                // S등급 x3
                "플라즈마 용융 토치", "클린 월드 엔드 캐논", "소형 블랙홀 압축기"
            };
            for (const auto& weaponName : cheatWeapons) {
                player->GetInventory().AddItem(cheatItems.GetItem(weaponName), 1);
            }
            notice = Color("[지급 완료] C~S 등급별 무기 세트 (11종)", "92");
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
    PlayBGM("loop_of_peace.wav");

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
                if (IsGameCleared()) { 
                    inMainMenu = false;
                    isProgramRunning = false;    // 바깥 while 루프도 종료 → 프로그램 완전 종료
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
                isProgramRunning = false;
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