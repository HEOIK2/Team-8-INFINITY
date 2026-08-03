#include "ui.h"
#include <iostream>

int main() {
	UI_Init();

	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";
	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";
	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";
	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";
	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";
	std::cout << DisplayWidth("─") << " " << DisplayWidth("│") << "\n";

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


	return 0;
	
}

