#pragma once					// 이 헤더가 한번만 포함되게 하는 문법. 여러 파일에서 이 헤더를 부르면 반복해서 불러서 개꼬이는 이상한 사태를 미연에 방지함. 그냥 헤더에는 쓴다고 기계적으로 외워라.
#include <string>

void UI_Init();													// Initialize 초기화. 이 넘은 콘솔이 ANSI 색상 코드를 해석하게 켜주는 것입니다. main 맨 앞에 딱 한번만 키면 돼요.
std::string Color(const std::string& s, const char* code);		// string은 std에서 꺼내온 것이기 때문에 std. const 상수. 참조 받으면 원본 건드릴 수도 있으니까 읽기 전용을 명시하는 것

std::string MakeGauge(int cur, int max, int slots);
// HP바 문자열 생성하는 것. 예시 MakeGauge(142, 200, 20) -> 현재값, 최대값, 칸수 -> ■■■■■■■■■■■ 142/ 200



void Line(const std::string& c = "=");; // 구분선 인자 없으면 =가 들어갑니다. 원하는 구분선 스타일을 넣어주세요.
void Title(const std::string& t); // 제목을 구분선 사이에 출력
void Clear(); // 화면 지우기
void Pause(); // 계속하려면 Enter

void ShowMainTitleArt(); // 메인타이틀아트입니다.