#include "ui.h"
#include <iostream>

int main() {
	UI_Init();
	ShowMainTitleArt();

	std::vector<std::string> body = {
	"",
	"녹슨 통조림 [금속]",
	"HP " + MakeGauge(51, 90, 10) + "   ATK 15",
	"",
	"기사단장 익 [깡통기사]",
	"HP " + MakeGauge(142, 200, 10) + "   ATK 40   Gold 85",
	""
	};

	std::vector<std::string> footer = {
		"> 깡통압축기(금속 C) 사용! 상성 적중! 90 데미지",
		"> 녹슨 통조림의 반격. 15 데미지"
	};

	DrawScreen("쓰레기장 - 전투 (Lv.3)", body, footer);
	Pause();




	Clear();
	BoxTop(50);
	BoxLine("녹슨 통조림 [금속]", 50);
	BoxLine("HP " + MakeGauge(51, 90, 10), 50);
	BoxDivider(50);
	BoxLine("기사단장 익", 50);
	BoxLine("Lv.3   ATK 40   Gold 85", 50);
	BoxBottom(50);
	Pause();

	Clear();
	std::cout << Color("빨간색", "31") << "\n";
	std::cout << "원래 색\n";
	std::cout << MakeGauge(142, 200, 20) << "\n";
	Title("쓰레기장 - 전투");
	std::cout << Color(MakeGauge(500, 1000, 50), "32") << "\n";
	Line("*");
	Pause();

	Line("=");
	Line("-");
	Line("_");
	Line("─");
	Line("━");
	Line("═");
	Line("▔");
	Line("▁");

	for (int c = 30; c <= 37; ++c) {
		std::cout << Color("■■■ 색 테스트 " + std::to_string(c), std::to_string(c).c_str()) << "\n";
	}
	for (int c = 90; c <= 97; ++c) {
		std::cout << Color("■■■ 색 테스트 " + std::to_string(c), std::to_string(c).c_str()) << "\n";
	}


	std::cout << "입력: \"HP\"           → " << DisplayWidth("HP") << "칸  (기대: 2)\n";
	std::cout << "입력: \"녹슨 통조림\"   → " << DisplayWidth("녹슨 통조림") << "칸  (기대: 11)\n";
	std::cout << "입력: \"Lv.3\"         → " << DisplayWidth("Lv.3") << "칸  (기대: 4)\n";
	std::cout << "입력: \"─\"            → " << DisplayWidth("─") << "칸  (기대: 2)\n";

	std::cout << "\n--- length()와 비교 ---\n";
	std::string s = "녹슨 통조림";
	std::cout << "입력: \"녹슨 통조림\"\n";
	std::cout << "  length()       = " << s.length() << " (바이트 수)\n";
	std::cout << "  DisplayWidth() = " << DisplayWidth(s) << " (화면 칸 수)\n";


	return 0;
	
}

