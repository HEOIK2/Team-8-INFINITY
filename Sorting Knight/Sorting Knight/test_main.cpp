#include "ui.h"
#include <iostream>

int main() {
	UI_Init();
	std::cout << Color("빨간색", "31") << "\n";
	std::cout << "원래 색\n";
	std::cout << MakeGauge(142, 200, 20) << "\n";
	return 0;

}