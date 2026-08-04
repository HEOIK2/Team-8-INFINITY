#pragma once					
#include <string>
#include <vector>

class Inventory;   // 전방 선언

void TypeTextAt(int row, int col, const std::string& text, const std::string& colorCode = "37", int delay = 40);
void ShowInventoryScreen(const Inventory& inv);

void UI_Init();
std::string Color(const std::string& s, const char* code);

std::string MakeGauge(int cur, int max, int slots);

void Line(const std::string& c = "=");
void Title(const std::string& t);
void Clear();
void Pause();

void ShowMainTitleArt();

int DisplayWidth(const std::string& s);

void BoxTop(int width);
void BoxBottom(int width);
void BoxDivider(int width);
void BoxLine(const std::string& text, int width);

// 기존 버전: 화면 전체를 박스로 그린다.
void DrawScreen(const std::string& title,
    const std::vector<std::string>& body,
    const std::vector<std::string>& footer);

// ★ 새로운 버전: 아스키 아트 영역이 추가된 분할 화면을 그린다.
void DrawScreen(const std::string& title,
    const std::vector<std::string>& body,
    const std::vector<std::string>& art,
    const std::vector<std::string>& footer);