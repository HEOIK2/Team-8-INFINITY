#pragma once					// 이 헤더가 한번만 포함되게 하는 문법. 여러 파일에서 이 헤더를 부르면 반복해서 불러서 개꼬이는 이상한 사태를 미연에 방지함. 그냥 헤더에는 쓴다고 기계적으로 외워라.
#include <string>
#include <vector>

class Inventory;   // 전방 선언
void ShowInventoryScreen(const Inventory& inv);

void UI_Init();													// Initialize 초기화. 이 넘은 콘솔이 ANSI 색상 코드를 해석하게 켜주는 것입니다. main 맨 앞에 딱 한번만 키면 돼요.
std::string Color(const std::string& s, const char* code);		// string은 std에서 꺼내온 것이기 때문에 std. const 상수. 참조 받으면 원본 건드릴 수도 있으니까 읽기 전용을 명시하는 것

std::string MakeGauge(int cur, int max, int slots);
// HP바 문자열 생성하는 것. 예시 MakeGauge(142, 200, 20) -> 현재값, 최대값, 칸수 -> ■■■■■■■■■■■ 142/ 200



void Line(const std::string& c = "=");; // 구분선 인자 없으면 =가 들어갑니다. 원하는 구분선 스타일을 넣어주세요.
void Title(const std::string& t); // 제목을 구분선 사이에 출력
void Clear(); // 화면 지우기
void Pause(); // 계속하려면 Enter

void ShowMainTitleArt(); // 메인타이틀아트입니다.

int DisplayWidth(const std::string& s); // // 문자열이 콘솔에서 차지하는 칸 수를 반환. 한글은 1글자당 2칸, 
										// 예) DisplayWidth("HP") = 2,  DisplayWidth("녹슨") = 4

// "허익 잠온다"
// ↓ ToWide(바이트 → 글자)
// [허][익][][잠][온][다]   6글자
// ↓ CharWidth(글자 → 칸)
// 2 + 2 + 1 + 2 + 2 + 2 = 11칸





// 박스 테두리. width는 전체 폭(칸 수) // 여기까지 함 이어서 하기.
void BoxTop(int width);                            // ┌────┐
void BoxBottom(int width);                         // └────┘
void BoxDivider(int width);                        // ├────┤
void BoxLine(const std::string& text, int width);  // │ 내용   │

// 화면 전체를 박스로 그린다.
// title  : 상단 제목
// body   : 본문 줄들
// footer : 하단 안내 줄들
void DrawScreen(const std::string& title,
    const std::vector<std::string>& body,
    const std::vector<std::string>& footer);