// rainbowtest.cpp
#include "ui.h"
#include <iostream>
#include <vector>

// 무지개 색 팔레트
static const char* RAINBOW[] = { "91", "93", "92", "96", "94", "95" };  // 빨노초청파보
static const int RAINBOW_N = 6;

// 가로 테두리. left/right는 양끝 모서리 문자, offset은 색 시작 위치
static void RainbowHLine(int width, const char* left, const char* right, int offset) {
    std::cout << Color(left, RAINBOW[offset % RAINBOW_N]);
    for (int i = 0; i < width - 2; ++i) {
        std::cout << Color("─", RAINBOW[(i + offset) % RAINBOW_N]);
    }
    std::cout << Color(right, RAINBOW[(width - 1 + offset) % RAINBOW_N]) << "\n";
}

// 내용 줄. 양쪽 세로변에 줄 번호에 따른 색
static void RainbowBoxLine(const std::string& text, int width, int row) {
    int used = 2 + DisplayWidth(text);
    int pad = width - used - 1;
    if (pad < 0) { pad = 0; }

    const char* c = RAINBOW[row % RAINBOW_N];
    std::cout << Color("│", c) << " " << text;
    for (int i = 0; i < pad; ++i) { std::cout << " "; }
    std::cout << Color("│", c) << "\n";
}

// 화면 전체를 무지개 테두리로
void DrawScreenRainbow(const std::string& title,
    const std::vector<std::string>& body,
    const std::vector<std::string>& footer,
    int width) {
    Clear();
    int row = 0;

    RainbowHLine(width, "┌", "┐", row++);
    RainbowBoxLine(title, width, row++);
    RainbowHLine(width, "├", "┤", row++);

    for (const std::string& line : body) { RainbowBoxLine(line, width, row++); }

    int used = 5 + (int)body.size() + (int)footer.size();
    for (int i = used; i < 30; ++i) { RainbowBoxLine("", width, row++); }

    RainbowHLine(width, "├", "┤", row++);
    for (const std::string& line : footer) { RainbowBoxLine(line, width, row++); }
    RainbowHLine(width, "└", "┘", row++);
}

int main() {
    UI_Init();

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

    DrawScreenRainbow("쓰레기장 - 전투 (Lv.3)", body, footer, 100);

    std::cin.get();
    return 0;
}