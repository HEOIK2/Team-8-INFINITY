#include "ui.h" // 내 ui 헤더를 먼저 얻고
#include <windows.h> // HANDLE, GetStdHandle, STDConsoleMode
#include <iostream> // cout과 cin을 위한

//	★	UI_Init, Color, MakeGauge 함수	★ (콘솔기능 ,글자색, 게이지)


// UI_Init(); == 콘솔에 색을 쓸 수 있게 켜주는 함수. main 맨 앞에서 한번만 호출합니다.
// windows.h가 정해놓은 절차라서 통째로 그냥 외워서 쓰면 됨다. 사실 이걸 이해하고 가려고 했는데 비트단위의 개념도 나오고 개오바인 것 같아서 일단 쓰겠습니다. 제가 지금 이거 이해할 단계가 아닙니다.

// [이 함수가 하는 일]
// 콘솔에 색깔을 쓸 수 있게 스위치를 켜준다. main 맨 앞에서 딱 한 번 호출.
// 이거 안 하면 색 코드가 글자로 그대로 찍힘 (←[31m 같은 게 보임).
//
// [왜 이렇게 복잡하냐]
// Windows 콘솔 설정이 "스위치 32개가 한 줄로 붙어있는 판" 형태라서,
// 원하는 스위치 하나만 켜려면 아래 3단계를 거쳐야 한다.
//   1) 콘솔을 조작할 권한(손잡이) 얻기
//   2) 지금 스위치들이 어떤 상태인지 읽기
//   3) 나머지는 그대로 두고, 내가 원하는 스위치 하나만 추가로 켜기
//
// [지금은 통째로 외워도 됨]
// Windows가 정해놓은 절차라 바꿀 수 없다. 그냥 복붙해서 쓰면 된다. 이런게 있다 정도만 이해하시오. 아마 개발자들도 복붙해서 그냥 쓸겁니다. 제가 교수가 되려는게 아니니까요... 색깔을 넣고 싶을뿐.

void UI_Init() {
	system("mode con: cols=120 lines=40"); // 콘솔 크기를 가로 100칸, 40줄로 고정해놓습니다.
	SetConsoleOutputCP(CP_UTF8); // UTF8로 콘솔 출력함.
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); //  콘솔 조작 권한 얻기
	DWORD mode = 0; //현재 설정값 담을 그릇
	if (GetConsoleMode(h, &mode)) { // 현재 설정 읽기 (성공하면 true가 되어서 mode) GetConsoleMode 함수의 자료형은 bool이다. 그니까 h에 입력 받고 &모드로 출력 뱉는 함수임. 
		SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);         // 얘가 켜져서 콘솔에 색을 쓸 수 있습니다.| = 비트 연산자인데, 기존 설정 유지하고 색상 기능만 추가로 켠다는 거랍니다.

	}
}

// ★ 글자 색 코드
//   [기본]  30검정 31빨강 32초록 33노랑 34파랑 35자홍 36청록 37흰색
//   [밝게]  90회색 91빨강 92초록 93노랑 94파랑 95자홍 96청록 97흰색
//   [배경]  40~47 (기본), 100~107 (밝게)  ← 글자색 +10
//   [효과]  1굵게 2흐리게 3기울임 4밑줄 7반전 9취소선
//   [리셋]  0
// 
// 
// Color(string& s, const char* code); 그냥 문자열 이어 붙여서 반환해주는 함수임. 근데 그 효과가 이제 색상이 입혀지는 것이지. 예시 \x1b[31mHP 12\x1b[0m -> 빨강색 켜 -> HP 12 -> 색 꺼
// \x1b = ESC == 아스키 27번 문자. 화면에 안 찍히고, "다음부터는 명령어다"를 알리는 신호. 글자모드에서 잠시 빠져 나온다는 뜻. \x == 뒤는 16진수 라는 뜻이래. [ ==명령 시작 괄호, 3
std::string Color(const std::string& s, const char* code) {
	return std::string("\x1b[") + code + "m" + s + "\x1b[0m"; 
}
// [\x1b[31m 구조 해부] 나중에 백퍼 까먹으니까 기억하려고.
// 
//   \x1b  = ESC 문자(아스키 27번). \x 는 "뒤는 16진수"라는 표기.
//           "지금부터 글자가 아니라 명령어다" 라는 신호.
//   [     = 명령 시작 괄호. 뒤에 값이 온다.
//   31    = 값 (색 번호)
//   m     = 명령 종류. m이면 "색 설정".
//           (H면 커서이동, J면 화면지우기 — 끝 글자가 명령을 정한다)




// MakeGauge(현재, 최대, 칸개수); 게이지 바 문자열 출력 할 때 쓰는 함수입니다이. 
std::string MakeGauge(int cur, int max, int slots) {

	// 0으로 나누기를 방지 max를 나누어야 할 때 max가 0이면 프로그램이 터짐. monster나 player에 maxHP라는 변수를 실수로 안넣으면 0으로 넘어옴. 바는 요상하게 나오는데, 프로그램은 안죽는다. 이런걸 방어코드라고 한다.
	if (max <= 0) { max = 1; } 

	// 음수 HP 방어. ex: HP: -132 이런거 방지하는 용도임.
	if (cur < 0) { cur = 0; } 

	// 최대치 초과 방어. 현재 체력이 최대체력보다 높아지는 230/100 이런걸 막는 용도임.
	if (cur > max) { cur = max; }

	// 그러니까 이 변수는 채울 칸 수를 나타냄. 그리고 HP가 1일 때 1 * 20 / 200 = 0.1 이니까 정수의 나눗셈이라 0이 되거든? 그럼 게이지가 0개가 돼 체력1이어도. 방지를 위해 max에 1을 빼주면 조금이라도 밀어올려서 1.09이 되겠네요
	// 이게 올림 나눗셈의 관용구라고 한다. 자주 쓰이는 것 같으니 인사하고 가자.
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
// 그리고 얻은 중요 정보. 20 * (100/200) 이런 식이 있다면 곱셈 나눗셈만 있으니까 곱할지 나눌지 순서를 바꿔도 원래 수학에서 계산은 같다. 개발 할 때는 곱셈을 먼저 한다. 100 * 20 / 200 이런식으로 
// 왜냐하면 정수의 나눗셈은
// 나머지를 버린다.



// ★	Line, Title, Clear, Pause 함수	★ (구분선, 타이틀, 화면지우기, 일시정지)



// 구분선 예시. Line() → "======..." / Line('-') → "------..."
// 구분선 인자 없으면 = 가 들어갑니다.원하는 구분선 스타일을 넣어주세요.		예시) Line('='); Line('*'); 등

const int LINE_WIDTH = 60; // 구분선의 길이. 여기만 바꾸면 전체가 바뀝니다. 한번 Line 함수쓰면 60개가 나옵미다. 조정가능

void Line(const std:: string& c) { // length가 바이트를 나타내준답니다. 라인별로 1바이트랑 3바이트짜리가 있어서 그걸 나눠서 판단하고 출력해줍니다.
	int charWidth = 1; // 하나 차지하는 그... 폭? 문자 하나 폭
	if (c.length() > 1) {
		charWidth = 2;
	}

	int count = LINE_WIDTH / charWidth;
	for (int i = 0; i < count; ++i) {
		std::cout << c;
	}
	std::cout << "\n";
}


// 타이틀인데 메인 타이틀 용이 아니라. 한 화면의 제목 줄임
// 예시 
//	============================================================
//	쓰레기장 - 전투
//	============================================================
void Title(const std::string& t) {
	Line("=");
	std::cout << " " << t << "\n";
	Line("=");
}

// 화면 지우기. 매 턴 시작에 부르면 화면이 안 쌓임. 
// 위에 UI_init으로 불러온 ANSI(American National Standards Institut 미국국립표준협회)를 써서 m는 색 설정, H는 커서이동 J는 화면지우기 K는 줄 지우기 ABCD는 커서 상하좌우키를 쓸 수 있다.
// 이걸로 화면로그가 깔끔해지는 효과~! 하하하

void Clear() {
	std::cout << "\x1b[2J\x1b[H";
}


// Pause 용도 — 화면이 휙 지나가는 걸 막는 것. 자동 전투라 로그가 순식간에 흘러가니까 턴마다 넣어야 쓸 수 있답니다. 그리고 Clear()로 화면을 지우기 직전에도 필요하다네요. 안 그러면 읽기 전에 사라집니다이.
void Pause() {
	std::cout << "\n계속하려면 Enter를 누르시오.";
	std::cin.ignore(); // 입력 버퍼에 남은 것을 비우는 함수.
	std::cin.get(); // Enter를 하나 기다림.
}


// 메인타이틀아트입니다.
// R(Raw string)을 쓰면 역슬래시를 그대로 출력합니다. 아마 우리 타이틀에는 없는 것 같은데 일단 그렇습니다. patorjk.com/software/taag에서 타이틀을 만들 수 있습니다.

// 메인 타이틀 아트. 게임 시작 시 한 번 호출.
// R"( )" = Raw string. 안의 역슬래시를 그대로 출력해줌.
// 아트는 patorjk.com/software/taag 에서 만들 수 있음.
void ShowMainTitleArt() {
	Clear();
	std::cout << "\n\n";

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

	std::cout << Color("							>재활용 공익근무요원: 요원의 마지막 기록\n", "92");
	std::cout << "				━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━";
	std::cout << Color("\n\n				   [ 코드 스니펫을 복사하겠습니다. 코드 스니펫을 붙여넣겠습니다. ]\n", "1");

	std::cin.get();
}



// <아래들은 테두리 박스를 만드는 함수입니다.>
// 
// 이 Towide 함수는 UTF-8 문자열(string)을 wide 문자열(wstring)으로 바꿔주는 일을 합니다. 왜냐하면 한글을 글자 단위로 세려면 wstring이어야 합니다. 
// 무슨 말인고 하니, string은 1바이트씩 저장합니다. UTF-8에서 한글 한 글자가 3바이트라서 글자 단위로 못셉니다.
// UTF-8에서는 "녹슨"  → [236][133][185][236][138][172]   한글자에 3칸이고요. 그니까 한 글자에 3바이트죠? wsting은 L"녹슨" → [녹][슨]   2칸입니다. 한 글자에 2칸입니다. 
// UFF-8에서는 H = 1byte P = 1byte 공백 = 1byte 녹 = 3byte 입니다. wstring에서는 H = 2바이트 P = 2바이트 녹 = 2바이트 슨 = 2바이트 다 똑같다. 그래서 한칸이 한글자로 보장됨. for로 꺼내쓰면 된다.
// 쉽게 말해서 이렇게 wsting이 되면 이 글자가 한글인가? 판정이 가능합니다.

static std::wstring ToWide(const std::string& s) { // 이건 이제 쉽습니다. 읽기전용으로 string& s참조합니다.
	if (s.empty()) {
		return L""; // empty는 빈 문자열이 들어오면 true가 됩니다. 빈 문자열일 때 L(wide string)""로 공백을 반환합니다. 그리고 return이니까 여기서 함수가 끝나는가보군요. 
		// 위의 if는 MultiByteToWideChar에 빈문자열을 넘기면 이상해 질 수도 있어서 막는 방어코드입니다.
	}
		int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str() , (int)s.size(), nullptr, 0); // 이거 변환하면 몇 글자 나오니? 라고 물어보는 함수야. 마지막에 두 인자가 nullptr, 0 이면 변환안하고 개수만 돌려 준대.
		std::wstring r(len, L'\0'); // 그리고 그 개수는 L에 담깁니다. 빈그릇 r을 만들어서 Len개 짜리 wstring을 만들고 \0으로 채워 놓습니다. 어차피 다음 줄에 덮어쓸거라서 뭘로 채우든 상관 없답니다.
		// \0은 널 문자입니다. 아스키 0번 화면에 아무것도 안찍힙니다. 그냥 여기서는 관습적으로 주로 사용되는 빈칸 채우기용 더미입니다.
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &r[0], len); // 실제로 변환해서 r에 채워넣는 줄입니다. nullptr, 0 자리에 &r[0] (r 그릇의 시작 주소값)과 len(크기)를 넣습니다. 여기다 이만큼 써줘라는 뜻.
	
		return r;
}


// 글자 하나가 콘솔에서 몇 칸을 먹는지 반환해주는 함수. 한글·CJK·전각은 2칸, 나머지는 1칸
static int CharWidth(wchar_t ch) { // wchar_t가 wide 글자를 하나 담는 타입이다. char의 2바이트 버전. ch는 그냥 변수이름이다. character.

	if ((ch >= 0x1100 && ch <= 0x115F) ||   // 한글 자모
		(ch >= 0x2500 && ch <= 0x257F) ||   // 박스 문자 ─│┌┐
		(ch >= 0x2580 && ch <= 0x259F) ||   // 블록 █▄▀
		(ch >= 0x25A0 && ch <= 0x25FF) ||   // 도형 ■□▶
		(ch >= 0x2E80 && ch <= 0xA4CF) ||   // CJK 기호·한자
		(ch >= 0xAC00 && ch <= 0xD7A3) ||   // 한글 완성형 (가~힣)
		(ch >= 0xFF00 && ch <= 0xFF60)) {   // 전각 영숫자
		return 2;
	}
	return 1;
}

// ToWide에서 받은걸 wchar_t ch로 하나씩 꺼내서 주워담고
int DisplayWidth(const std::string& s) {
	int w = 0;
	for (wchar_t ch : ToWide(s)) { // 오른쪽에 있는 것들을 하나씩 꺼내서 왼쪽에 담아라.
		w += CharWidth(ch);
	}
	return w;
}

// 흐름:
// DisplayWidth("허익 의모험") 호출
// 안에서 ToWide("허익 의모험") → L"허익 의모험" (6글자짜리 wstring)
// for로 글자 하나씩 꺼냄 : 허, 익, 공백, 의, 모, 험
// 각각 CharWidth로 판정 : 2, 2, 1, 2, 2, 2
// 다 더하면 11
// 
// 정리:
// wstring으로 바꾼다 → 글자 단위로 꺼낼 수 있게
// 글자를 하나씩 꺼낸다 → for
// 각각 1칸인지 2칸인지 판정한다 → CharWidth
// 다 더한다 → 총 칸 수


// 아래는 박스입니다.             각 Box 함수에 width 너비를 인자로 받고 그 수만큼 길게 합니다. for에서 2를 빼주는 이유는 위 아래 꺽쇠를 뺐기 때문이겠죠.
// ┌──────────┐
void BoxTop(int width) {
	std::cout << "┌";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┐\n";
}

// └──────────┘
void BoxBottom(int width) {
	std::cout << "└";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┘\n";
}

// ├──────────┤
void BoxDivider(int width) {
	std::cout << "├";
	for (int i = 0; i < width - 2; ++i) { std::cout << "─"; }
	std::cout << "┤\n";
}

// │ 내용        │
// 내용 폭을 재서 남는 만큼 공백을 채워 오른쪽 변을 맞춤
void BoxLine(const std::string& text, int width) {
	int used = 2 + DisplayWidth(text);      // 왼쪽 변(1) + 공백(1) + 내용
	int pad = width - used - 1;             // 남은 칸 (오른쪽 변 1칸 제외)
	if (pad < 0) { pad = 0; }

	std::cout << "│ " << text;
	for (int i = 0; i < pad; ++i) { std::cout << " "; }
	std::cout << "│\n";
}

const int BOX_WIDTH = 100;   // 콘솔 폭에 맞춤 (UI_Init에서 cols=120)


	void DrawScreen(const std::string & title,
		const std::vector<std::string>&body,
		const std::vector<std::string>&footer) {
		Clear();
		BoxTop(BOX_WIDTH);
		BoxLine(title, BOX_WIDTH);
		BoxDivider(BOX_WIDTH);

		for (const std::string& line : body) { BoxLine(line, BOX_WIDTH); }

		// 남는 줄을 빈 줄로 채워서 하단 영역을 아래로 밀어냄
		int used = 5 + (int)body.size() + (int)footer.size();
		for (int i = used; i < 30; ++i) { BoxLine("", BOX_WIDTH); }

		BoxDivider(BOX_WIDTH);
		for (const std::string& line : footer) { BoxLine(line, BOX_WIDTH); }
		BoxBottom(BOX_WIDTH);
	}


