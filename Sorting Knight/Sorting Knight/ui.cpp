#include "ui.h" 
#include <windows.h> 
#include <iostream> 

void UI_Init() {
	system("mode con: cols=120 lines=40");
	SetConsoleOutputCP(CP_UTF8);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	if (GetConsoleMode(h, &mode)) {
		SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
}

std::string Color(const std::string& s, const char* code) {
	return std::string("\x1b[") + code + "m" + s + "\x1b[0m";
}

std::string MakeGauge(int cur, int max, int slots) {
	if (max <= 0) { max = 1; }
	if (cur < 0) { cur = 0; }
	if (cur > max) { cur = max; }

	int filled = (cur * slots + max - 1) / max;

	std::string bar;
	for (int i = 0; i < slots; ++i) {
		if (i < filled) {
			bar += "■";
		}
		else bar += ".";

	}
	return bar;
}

const int LINE_WIDTH = 60;

void Line(const std::string& c) {
	int charWidth = 1;
	if (c.length() > 1) {
		charWidth = 2;
	}
	int count = LINE_WIDTH / charWidth;
	for (int i = 0; i < count; ++i) {
		std::cout << c;
	}
	std::cout << "\n";
}

void Title(const std::string& t) {
	Line("=");
	std::cout << " " << t << "\n";
	Line("=");
}

void Clear() {
	std::cout << "\x1b[2J\x1b[H";
}

void Pause() {
	std::cout << "\n계속하려면 Enter를 누르시오.";
	std::cin.ignore();
	std::cin.get();
}

// 메인 타이틀 아트. 게임 시작 시 한 번 호출.
void ShowMainTitleArt() {
	Clear();

	// 수직 중앙 정렬을 위해 위쪽에 빈 줄을 넉넉히 깔아줍니다. (총 40줄 중 비율 고려)
	std::cout << "\n\n\n\n\n\n\n\n";

	// 가로 120칸에 맞춰 좌측 여백을 26칸 정도로 띄워 중앙에 배치했습니다.
	std::cout << Color(R"(
                          ██████╗ ███████╗ ██████╗██╗   ██╗ ██████╗██╗     ██╗███╗   ██╗ ██████╗ 
                          ██╔══██╗██╔════╝██╔════╝╚██╗ ██╔╝██╔════╝██║     ██║████╗  ██║██╔════╝ 
                          ██████╔╝█████╗  ██║      ╚████╔╝ ██║     ██║     ██║██╔██╗ ██║██║  ███╗
                          ██╔══██╗██╔══╝  ██║       ╚██╔╝  ██║     ██║     ██║██║╚██╗██║██║   ██║
                          ██║  ██║███████╗╚██████╗   ██║   ╚██████╗███████╗██║██║ ╚████║╚██████╔╝
                          ╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝    ╚═════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ 

                          ██╗  ██╗███╗   ██╗██╗ ██████╗ ██╗  ██╗████████╗
                          ██║ ██╔╝████╗  ██║██║██╔════╝ ██║  ██║╚══██╔══╝
                          █████╔╝ ██╔██╗ ██║██║██║  ███╗███████║   ██║   
                          ██╔═██╗ ██║╚██╗██║██║██║   ██║██╔══██║   ██║   
                          ██║  ██╗██║ ╚████║██║╚██████╔╝██║  ██║   ██║   
                          ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   
)", "36");

	// 부제목과 하단 선, 시작 안내 문구도 중앙에 맞게 정렬
	std::cout << Color("\n                          > 재활용 공익근무요원: 요원의 마지막 기록\n\n", "92");
	std::cout << "     ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
	std::cout << Color("\n                                            [ Enter 키를 눌러 시작 ]\n", "90");

	std::cin.get();
}

// ★ 테두리 찐빠 수정용 방어 로직 (색상 코드 무시)
std::string StripANSI(const std::string& s) {
	std::string result;
	bool inANSI = false;
	for (char c : s) {
		if (c == '\x1b') {
			inANSI = true;
		}
		else if (inANSI) {
			if (c == 'm') inANSI = false;
		}
		else {
			result += c;
		}
	}
	return result;
}

static std::wstring ToWide(const std::string& s) {
	if (s.empty()) { return L""; }
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &r[0], len);
	return r;
}

// 글자 하나가 콘솔에서 몇 칸을 먹는지 반환해주는 함수.
static int CharWidth(wchar_t ch) {
	if ((ch >= 0x1100 && ch <= 0x115F) ||   // 한글 자모
		(ch == 0x203B) ||                   // ★ 추가: 당구장 마크(※)는 2칸으로 계산!
		(ch >= 0x2E80 && ch <= 0xA4CF) ||   // CJK 기호·한자
		(ch >= 0xAC00 && ch <= 0xD7A3) ||   // 한글 완성형 (가~힣)
		(ch >= 0xFF00 && ch <= 0xFF60)) {   // 전각 영숫자
		return 2;
	}
	return 1;
}

int DisplayWidth(const std::string& s) {
	std::string cleanStr = StripANSI(s); // ★ 색상 코드를 무시하고 너비 계산
	int w = 0;
	for (wchar_t ch : ToWide(cleanStr)) {
		w += CharWidth(ch);
	}
	return w;
}

void BoxTop(int width) {
	std::cout << "┌";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┐\n";
}

void BoxBottom(int width) {
	std::cout << "└";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┘\n";
}

void BoxDivider(int width) {
	std::cout << "├";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┤\n";
}

void BoxLine(const std::string& text, int width) {
	int used = 2 + DisplayWidth(text);
	int pad = width - used - 1;
	if (pad < 0) { pad = 0; }

	std::cout << "│ " << text;
	for (int i = 0; i < pad; ++i) { std::cout << " "; }
	std::cout << "│\n";
}

const int BOX_WIDTH = 117;

// 기존 버전: 화면 전체를 박스로 그린다.
void DrawScreen(const std::string& title,
	const std::vector<std::string>& body,
	const std::vector<std::string>& footer) {
	Clear();
	BoxTop(BOX_WIDTH);
	BoxLine(title, BOX_WIDTH);
	BoxDivider(BOX_WIDTH);

	for (const std::string& line : body) { BoxLine(line, BOX_WIDTH); }

	int used = 5 + (int)body.size() + (int)footer.size();
	for (int i = used; i < 38; ++i) { BoxLine("", BOX_WIDTH); }

	BoxDivider(BOX_WIDTH);
	for (const std::string& line : footer) { BoxLine(line, BOX_WIDTH); }
	BoxBottom(BOX_WIDTH);
}

// ★ 새로운 버전: 아스키 아트 영역이 추가된 분할 화면을 그린다.
void DrawScreen(const std::string& title,
	const std::vector<std::string>& body,
	const std::vector<std::string>& art,
	const std::vector<std::string>& footer) {

	Clear();
	BoxTop(BOX_WIDTH);
	BoxLine(title, BOX_WIDTH);
	BoxDivider(BOX_WIDTH);

	// 1. Body (본문 영역 출력)
	for (const std::string& line : body) { BoxLine(line, BOX_WIDTH); }

	// 2. 구분선 (본문과 아트를 나누는 선, 빈 아트라도 무조건 화면을 예쁘게 분할해줌)
	BoxDivider(BOX_WIDTH);

	// 3. Art (본문 구분선 바로 밑에 아트 출력)
	for (const std::string& line : art) { BoxLine(line, BOX_WIDTH); }

	// 4. 빈칸(여백) 채우기 (아트를 다 그리고 남는 공간을 맨 밑으로 밀어버림)
	int used = 6 + (int)body.size() + (int)art.size() + (int)footer.size();
	for (int i = used; i < 38; ++i) { BoxLine("", BOX_WIDTH); }

	// 5. Footer (하단 안내 출력)
	BoxDivider(BOX_WIDTH);
	for (const std::string& line : footer) { BoxLine(line, BOX_WIDTH); }

	BoxBottom(BOX_WIDTH);
}