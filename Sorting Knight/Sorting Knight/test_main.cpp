#include "ui.h"
#include <iostream>

int main() {
	UI_Init();
	ShowMainTitleArt();



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

	return 0;
	
}

