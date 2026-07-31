#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>

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



// [헬퍼 함수] 입력 오류(문자 입력 등) 방지
void ClearInputBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
}


// [콘텐츠 로직 함수 정의] 

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
void InitializeGame() {

}

// 상점 호출 
void EnterShopMenu() {

}

// 전투 호출
void EnterBattleMenu() {

}

// 인벤토리
void OpenInventory() {

}
std::vector<std::pair<Item, int>> myItems;

// 프로그램 진입점 (main 함수)

int main() {
    bool isProgramRunning = true;

    // [Outer Loop] 타이틀 화면 ↔ 메인 게임
    while (isProgramRunning) {
		Player* player = new Player("플레이어", Job::Cleaner); // 임시로 기본 캐릭터 생성 (이름/직업은 나중에 입력받도록 수정 가능)
		Monster* monster = new Monster("몬스터", MonsterType::NONE , 30, 10,10,10); // 임시로 기본 몬스터 생성 (이름/레벨/체력/공격력)

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
        InitializeGame();

        // 3. [Inner Loop] 인게임 메인 메뉴 루프
        bool inMainMenu = true;
        while (inMainMenu) {
            std::cout << "\n========== [ 메인 메뉴 ] ==========" << std::endl;
            std::cout << "1. 전투" << std::endl;
            std::cout << "2. 상점" << std::endl;
            std::cout << "3. 인벤토리" << std::endl;
            std::cout << "0. 종료 (타이틀로 돌아가기)" << std::endl;
            std::cout << "===================================" << std::endl;
            std::cout << "선택: ";

            int mainChoice;
            std::cin >> mainChoice;
            ClearInputBuffer();
            
            switch (mainChoice) {
            case 1:
				 // 전투에 사용할 아이템 목록 (예시)
                // StartBattle(Player* player, Monster* monster, std::vector<std::pair<Item, int>>& items)
                StartBattle(player, monster, {myItems}); // 전투 호출 (예시함수)
                break;
            case 2:
                EnterShopMenu(); // 상점 호출 (예시함수)
                break;
            case 3:
                OpenInventory(); // 인벤토리 호출 (예시함수)
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
    }

    return 0;
}