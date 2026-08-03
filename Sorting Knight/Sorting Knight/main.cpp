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
#include "UI.h"



// [헬퍼 함수] 입력 오류(문자 입력 등) 방지
void ClearInputBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
}

// 타이틀 화면을 출력 and 사용자 입력을 받는 함수 (아마 UI 호출해서 들어갈 것)
int ShowTitleMenu() {
    std::cout << "\n===================================" << std::endl;
    std::cout << "           [ 타이틀 화면 ]           " << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "1. 게임 시작" << std::endl;
    std::cout << "2. 게임 종료" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "선택: ";

    int choice;
    std::cin >> choice;
    ClearInputBuffer();

    return choice;
}

// 플레이어 초기 세팅 함수
// 유효한 입력을 받을 때까지 재시도하고, 생성된 Player*를 반환합니다.
Player* InitializeGame() {
    std::string name;
    int cnt = 0;

    std::cout << "플레이어 이름과 직업을 정해주세요" << std::endl;
    std::cout << "직업: 1. 청소부 2. 환경미화원 3. 분리수거전문가 4. 재활용기사" << std::endl;

    // 이름 입력
    while (true) {
        std::cout << "이름: ";
        if (std::cin >> name) {
            ClearInputBuffer();
            if (!name.empty()) break;
        }
        else {
            ClearInputBuffer();
        }
        std::cout << "유효한 이름을 입력하세요.\n";
    }

    // 직업 입력 (1~4)
    while (true) {
        std::cout << "직업 번호(1-4): ";
        if (std::cin >> cnt) {
            ClearInputBuffer();
            if (cnt >= 1 && cnt <= 4) break;
        }
        else {
            ClearInputBuffer();
        }
        std::cout << "유효한 숫자(1~4)를 입력하세요.\n";
    }

    switch (cnt) {
    case 1:
        return new Player(name, Job::Cleaner); // 임시로 기본 캐릭터 생성
    case 2:
        return new Player(name, Job::StreetCleaner);
    case 3:
        return new Player(name, Job::RecycleExpert);
    case 4:
        return new Player(name, Job::RecycleTech);
    default:
        return new Player(name, Job::Cleaner);
    }
}

// 직업별 초기 아이템 지급
void GiveInitialItems(Player* player, ItemManager& itemManager) {

    switch (player->GetJob()) {
    case Job::Cleaner:
        player->GetInventory().AddItem(itemManager.GetItem("대나무 빗자루 (C)"), 1);
        break;
    case Job::StreetCleaner:
        player->GetInventory().AddItem(itemManager.GetItem("나무 젓가락 (C)"), 1);
        break;
    case Job::RecycleExpert:
        player->GetInventory().AddItem(itemManager.GetItem("평범한 가위 (C)"), 1);
        break;
    case Job::RecycleTech:
        player->GetInventory().AddItem(itemManager.GetItem("분리수거 집게 (C)"), 1);
        break;
    default:
        break;
    }
}


// 프로그램 진입점 (main 함수)

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    bool isProgramRunning = true;

    // [Outer Loop] 타이틀 화면 ↔ 메인 게임
    while (isProgramRunning) {

        // 1. 타이틀 화면 선택
        int titleChoice = ShowTitleMenu();

        if (titleChoice == 2) {
            std::cout << "게임을 완전히 종료합니다. (바탕화면으로)" << std::endl;
            isProgramRunning = false;
            break;
        }
        else if (titleChoice != 1) {
            std::cout << "잘못된 입력입니다." << std::endl;
            continue;
        }

        // 2. 캐릭터 생성 및 초기 세팅
        // 
        // 2 - 0. 아이템 목록 (전투에서 사용할 아이템)
        ItemManager itemManager; // 아이템 매니저 인스턴스 생성

        // 2 - 1. InitializeGame은 생성된 Player*를 반환합니다.
        Player* player = InitializeGame();
        if (!player) {
            std::cout << "플레이어 생성에 실패했습니다. 타이틀로 돌아갑니다." << std::endl;
            continue;
        }
        // 2 - 2. 초기 아이템 지급 (예시)
        GiveInitialItems(player, itemManager);
        // 2 - 3 . 상점 초기화 (전역 포인터에 player와 itemManager 주소 저장)
        InitShop(player, &itemManager);

        // 3. [Inner Loop] 인게임 메인 메뉴 루프
        bool inMainMenu = true;
        while (inMainMenu) {
            std::cout << "\n========== [ 메인 메뉴 ] ==========" << std::endl;
            std::cout << "1. 전투" << std::endl;
            std::cout << "2. 상점" << std::endl;
            std::cout << "3. 인벤토리" << std::endl;
            std::cout << "4. 플레이어 스탯" << std::endl;
            std::cout << "9. ???" << std::endl;
            std::cout << "0. 종료 (타이틀로 돌아가기)" << std::endl;
            std::cout << "===================================" << std::endl;
            std::cout << "선택: ";

            int mainChoice;
            std::cin >> mainChoice;
            ClearInputBuffer();

            switch (mainChoice) {
            case 1:
                EnterBattle(player); // 전투 호출 (예시함수)
                break;
            case 2:
                EnterShopMenu(); // 상점 호출 (예시함수)
                break;
            case 3:
                player->GetInventory().Show();
                break;
            case 4:
                player->PrintStatus(); // 상태창 호출 (예시함수)               
                break;
            case 9:
                std::cout << "1. 개발자용 : 경험치 1000 획득" << std::endl;
                std::cout << "2. 개발자용 : 10000G 획득" << std::endl;
                std::cout << "3. 나가기 " << std::endl;
                int DebugChoice;
                while (true) {
                    std::cin >> DebugChoice;
                    if (DebugChoice == 1 || DebugChoice == 2 || DebugChoice == 3) {
                        break;
                    }
                    std::cout << "잘못된 입력입니다. 다시 선택하세요: ";
                }
                switch (DebugChoice) {
                case 1:
                    player->GainExp(1000);
                    break;
                case 2:
                    player->SetGold(player->GetGold() + 10000);
                    break;
                }
                break;
            case 0:
                std::cout << "\n타이틀 화면으로 돌아갑니다..." << std::endl;
                inMainMenu = false; // 메인 루프 탈출 (타이틀로 회귀)
                break;
            default:
                std::cout << "잘못된 입력입니다." << std::endl;
                break;
            }
        }

        // 플레이어/몬스터 메모리 정리 (메뉴에서 타이틀로 돌아갈 때)
        delete player;
        player = nullptr;
    }
    return 0;
}