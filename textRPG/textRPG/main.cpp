#include <iostream>
#include <string>
// #include "Player.h"
// #include "BattleManager.h"
// #include "ShopManager.h"

using namespace std;

// ==========================================
// [헬퍼 함수] 입력 오류(문자 입력 등) 방지
// ==========================================
void ClearInputBuffer() {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

// ==========================================
// [각 콘텐츠별 함수 선언] (실제로는 Manager 클래스 등에 들어갈 내용)
// ==========================================
void EnterShopMenu();      // 상점 진입
void EnterBattleMenu();    // 전투 진입 (내부에서 전투 완료 후 상점 방문 처리)
void OpenInventory();      // 인벤토리 열기
int  ShowTitleMenu();      // 타이틀 화면 출력 및 선택 반환
void InitializeGame();     // 캐릭터 생성 등 게임 시작 전 세팅
void RunMainMenuLoop();    // 인게임 메인 메뉴 루프

// ==========================================
// 🌟 1. 프로그램 진입점 (main)
// ==========================================
int main() {
    bool isProgramRunning = true;

    // [Outer Loop] 타이틀 화면 ↔ 메인 게임
    while (isProgramRunning) {
        // 1. 타이틀 화면 출력 및 선택 반환
        int titleChoice = ShowTitleMenu();

        if (titleChoice == 2) {
            cout << "게임을 완전히 종료합니다. (바탕화면으로)" << endl;
            isProgramRunning = false;
            break;
        }
        else if (titleChoice != 1) {
            continue;
        }

        // 2. 캐릭터 생성 및 초기 세팅
        InitializeGame();

        // 3. 인게임 메인 루프 진입 (0번 종료를 누르면 여기로 빠져나와 다시 타이틀로 감)
        RunMainMenuLoop();
    }

    return 0;
}


// ==========================================
// 🌟 2. 함수 세부 구현 (각 분기 로직)
// ==========================================

// 타이틀 화면을 보여주고 유저의 선택(1 or 2)을 반환하는 함수
int ShowTitleMenu() {
    cout << "\n===================================" << endl;
    cout << "           [ 타이틀 화면 ]           " << endl;
    cout << "===================================" << endl;
    cout << "1. 게임 시작" << endl;
    cout << "2. 게임 종료" << endl;
    cout << "===================================" << endl;
    cout << "선택: ";

    int choice;
    cin >> choice;
    ClearInputBuffer();

    return choice;
}

// 플레이어 이름, 직업 등을 입력받고 게임을 세팅하는 함수
void InitializeGame() {
    cout << "\n--- [ 캐릭터 생성 ] ---" << endl;
    // (이름 입력받기, 직업 입력받기, 무기 지급 등 Player 객체 생성 로직)
    cout << "게임을 시작합니다!" << endl;
}

// 전투 메뉴 로직 (종료 후 상점 방문 분기 포함)
void EnterBattleMenu() {
    cout << "\n--- [ 전투 메뉴 ] ---" << endl;
    cout << "1. 잡몹방 | 2. 중간보스 | 3. 보스" << endl;
    // (선택 및 전투 진행 로직)
    cout << "전투가 종료되었습니다." << endl;

    // 전투 후 상점 방문 분기
    cout << "\n전투가 끝났습니다. 상점에 방문하시겠습니까? (1: 네 / 2: 아니오): ";
    int visitShop;
    cin >> visitShop;
    ClearInputBuffer();

    if (visitShop == 1) {
        EnterShopMenu(); // 상점 함수로 즉시 연결
    }
}

// 상점 메뉴 로직
void EnterShopMenu() {
    cout << "\n--- [ 상 점 ] ---" << endl;
    cout << "1. 기본템 구매 | 2. 소비아이템 구매 | 3. 합성 가챠" << endl;
    // (상점 이용 로직)
    cout << "상점 이용을 마쳤습니다." << endl;
}

// 인벤토리 로직
void OpenInventory() {
    cout << "\n--- [ 인벤토리 ] ---" << endl;
    // (플레이어 인벤토리 출력 및 장비 장착/사용 로직)
    cout << "인벤토리를 닫습니다." << endl;
}

// 인게임 메인 메뉴를 돌려주는 핵심 루프 함수
void RunMainMenuLoop() {
    bool inMainMenu = true;

    // [Inner Loop] 메인 메뉴
    while (inMainMenu) {
        cout << "\n========== [ 메인 메뉴 ] ==========" << endl;
        cout << "1. 전투" << endl;
        cout << "2. 상점" << endl;
        cout << "3. 인벤토리" << endl;
        cout << "0. 종료 (타이틀로 돌아가기)" << endl;
        cout << "===================================" << endl;
        cout << "선택: ";

        int mainChoice;
        cin >> mainChoice;
        ClearInputBuffer();

        switch (mainChoice) {
        case 1:
            EnterBattleMenu();
            break;
        case 2:
            EnterShopMenu();
            break;
        case 3:
            OpenInventory();
            break;
        case 0:
            cout << "\n타이틀 화면으로 돌아갑니다..." << endl;
            inMainMenu = false; // 루프 탈출
            break;
        default:
            cout << "잘못된 입력입니다." << endl;
            break;
        }
    }
}