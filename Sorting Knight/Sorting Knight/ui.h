#pragma once
#include <string>
#include <vector>

// ============================================================
// ui.h / ui.cpp
// ------------------------------------------------------------
// JRPG 스타일 화면(제목 + 캐릭터 영역 + 커맨드 목록 + 로그)을
// 그려주는 전용 함수 모음. 화면은 항상 "지금 콘솔 창 크기"에
// 맞춰서 그려짐 (콘솔 창을 키우면 다음 화면부터 같이 커짐).
//
// [주요 함수]
//   UI_Init()            프로그램 시작 시 한 번 호출 (필수)
//   UI_RunMenuSelector() 방향키 ↑/↓ + Enter로 항목을 고르는 화면
//   UI_ShowMessage()     메시지 확인 화면 (Enter로 진행)
//   UI_ShowStory()       스토리/연출용 전체 화면 (색깔 줄 단위)
//   UI_ShowSplash()      로고 스플래시
//   UI_ShowInputScreen() 박스 안에서 텍스트 입력 받기용 화면
//   UI_Colored()         로그 문자열의 일부만 색칠할 때 사용
//   UI_MakeGauge()       HP바("■■■□□ 106/150") 문자열 생성
// ============================================================

// 글자 색
enum class UIColor {
    Default,
    White,
    Gray,
    Red,
    Green,
    Yellow,
    Blue,
    Cyan,
    Magenta
};

// 색이 있는 한 줄 (예: "HP 106/150" 을 빨간색으로 표시)
struct UILine {
    std::string text;
    UIColor color = UIColor::Default;
};

// 화면에 등장하는 캐릭터/몬스터 한 명
struct ActorDisplay {
    std::vector<std::string> art;    // 아스키아트 (줄 단위 문자열)
    std::string name;
    UIColor nameColor = UIColor::Green;
    std::vector<UILine> statLines;   // 이름 아래에 세로로 표시할 스탯 줄들
};

// 커맨드 목록 항목 하나. color를 지정하면 그 색으로 표시됨 (등급 색 구분 등).
struct MenuOption {
    std::string label;
    UIColor color = UIColor::Default;
};

// 콘솔에서 색/커서 이동(ANSI 이스케이프)이 동작하도록 켠다.
// main() 맨 앞에서 딱 한 번만 호출하면 됨.
void UI_Init();

// 화면 전체(제목 + 캐릭터 영역 + 구분선 + 커맨드 목록 + 로그)를 그린다.
// selectedIndex 는 지금 커서가 가 있는 항목 번호 (강조 표시됨). 없으면 -1.
void UI_RenderScreen(const std::string& title,
                      const std::vector<ActorDisplay>& actors,
                      const std::vector<MenuOption>& commands,
                      int selectedIndex,
                      const std::string& logMessage);

// 화면을 그리면서 방향키(↑/↓)로 항목을 고르고 Enter로 확정받는다.
// 항목이 화면보다 많으면 자동으로 스크롤됨.
// 반환값: 고른 항목의 인덱스 (0부터 시작). Esc를 누르면 -1 반환(취소).
int UI_RunMenuSelector(const std::string& title,
                        const std::vector<ActorDisplay>& actors,
                        const std::vector<MenuOption>& commands,
                        const std::string& logMessage = "");

// 버튼 없이 메시지만 보여주고, Enter를 누르면 넘어가는 화면.
// 데미지 로그, 승리/패배 안내 같은 "확인만 하면 되는" 화면에 사용.
// logMessage 안에 UI_Colored()로 만든 색깔 조각을 섞어도 됨.
void UI_ShowMessage(const std::string& title,
                     const std::vector<ActorDisplay>& actors,
                     const std::string& message);

// 스토리/연출 전용 화면. 줄 단위로 색을 지정해서 화면 중앙에 보여준다.
// (오프닝, 엔딩, 보스 대사, 상태 확인 등 텍스트가 긴 화면에 사용)
void UI_ShowStory(const std::string& title,
                   const std::vector<UILine>& lines);

// 화면을 그리고 promptText를 로그 영역에 띄운 뒤, 그 바로 뒤에 커서를 남겨둔다.
// 실제 텍스트 입력(std::cin 등)은 호출부에서 그대로 하면 됨 - 입력한 글자가
// 박스 안(커서 위치)에 자연스럽게 표시된다.
void UI_ShowInputScreen(const std::string& title,
                         const std::vector<ActorDisplay>& actors,
                         const std::string& promptText);

// 화면을 꽉 채우는 로고/스플래시를 보여주고 아무 키나 누르면 넘어간다.
void UI_ShowSplash(const std::wstring& art);

// 문자열의 일부만 색칠할 때 사용. 예:
//   log += UI_Colored("42 데미지!", UIColor::Red) + " 대상 잔여 내구 8";
std::string UI_Colored(const std::string& text, UIColor color);

// HP바 문자열 생성. 예: UI_MakeGauge(106, 150, 10) -> "■■■■■■■□□□ 106/150"
std::string UI_MakeGauge(int current, int max, int slots);
