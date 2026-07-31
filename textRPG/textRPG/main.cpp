#include <iostream>
#include <string>

//include <브라블라>
//#include "player.h"등등 더 추가하기


using namespace std;

// [헬퍼 함수] 입력 오류(문자 입력 등) 방지
void ClearInputBuffer() {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
    }
}


// [콘텐츠 로직 함수 정의] 

// 타이틀 화면을 출력 and 사용자 입력을 받는 함수 (아마 UI 호출해서 들어갈 것)
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


// 프로그램 진입점 (main 함수)

int main() {
    bool isProgramRunning = true;

    // [Outer Loop] 타이틀 화면 ↔ 메인 게임
    while (isProgramRunning) {

        // 1. 타이틀 화면 선택
        int titleChoice = ShowTitleMenu();

        if (titleChoice == 2) {
            cout << "게임을 완전히 종료합니다. (바탕화면으로)" << endl;
            isProgramRunning = false;
            break;
        }
        else if (titleChoice != 1) {
            cout << "잘못된 입력입니다." << endl;
            continue;
        }

        // 2. 캐릭터 생성 및 초기 세팅
        InitializeGame();

        // 3. [Inner Loop] 인게임 메인 메뉴 루프
        bool inMainMenu = true;
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
				EnterBattleMenu(); // 전투 호출 (예시함수)
                break;
            case 2:
                EnterShopMenu(); // 상점 호출 (예시함수)
                break;
            case 3:
                OpenInventory(); // 인벤토리 호출 (예시함수)
                break;
            case 0:
                cout << "\n타이틀 화면으로 돌아갑니다..." << endl;
                inMainMenu = false; // 메인 루프 탈출 (타이틀로 회귀)
                break;
            default:
                cout << "잘못된 입력입니다." << endl;
                break;
            }
        } 
    } 

    return 0;
}