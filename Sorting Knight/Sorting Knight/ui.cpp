#define NOMINMAX
#include "ui.h"
#include <windows.h>
#include <conio.h>
#include <sstream>

// ============================================================
// 내부 전용 헬퍼 (ui.h 에는 노출 안 됨)
// ------------------------------------------------------------
// 화면은 std::wstring(유니코드) 버퍼에 그린 다음 WriteConsoleW로
// 한 번에 출력한다. 이렇게 하면:
//   - 박스 문자(─│┌┐└┘ 등) 같은 진짜 유니코드 글자를 안전하게 쓸 수 있고
//   - 기존 한글(CP949로 컴파일됨)도 MultiByteToWideChar로 변환해서
//     같이 섞어 쓸 수 있다 (콘솔 코드페이지를 안 바꾸므로 다른 곳의
//     한글 출력이 깨지지 않음).
// ============================================================
namespace {

    std::wstring ToWide(const std::string& s) {
        if (s.empty()) return std::wstring();
        int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), nullptr, 0);
        std::wstring result(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), &result[0], len);
        return result;
    }

    // 콘솔 창이 실제로 화면에 보여주는 가로/세로 칸 수
    void GetConsoleSize(int& cols, int& rows) {
        cols = 80;
        rows = 30;
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
            cols = info.srWindow.Right - info.srWindow.Left + 1;
            rows = info.srWindow.Bottom - info.srWindow.Top + 1;
        }
    }

    // 한글/한자 등 "2칸짜리" 글자 판정 (정렬 계산용)
    int CharWidth(wchar_t ch) {
        if ((ch >= 0x1100 && ch <= 0x115F) ||
            (ch >= 0x2E80 && ch <= 0xA4CF) ||
            (ch >= 0xAC00 && ch <= 0xD7A3) ||
            (ch >= 0xF900 && ch <= 0xFAFF) ||
            (ch >= 0xFF00 && ch <= 0xFF60) ||
            (ch >= 0xFFE0 && ch <= 0xFFE6)) {
            return 2;
        }
        return 1;
    }

    int DisplayWidth(const std::wstring& s) {
        int w = 0;
        for (wchar_t ch : s) w += CharWidth(ch);
        return w;
    }

    const char* ColorCodeNarrow(UIColor color) {
        switch (color) {
        case UIColor::White:   return "37";
        case UIColor::Gray:    return "90";
        case UIColor::Red:     return "31";
        case UIColor::Green:   return "32";
        case UIColor::Yellow:  return "33";
        case UIColor::Blue:    return "34";
        case UIColor::Cyan:    return "36";
        case UIColor::Magenta: return "35";
        default:               return "39";
        }
    }

    std::wstring ColorCode(UIColor color) {
        std::string narrow = ColorCodeNarrow(color);
        return std::wstring(narrow.begin(), narrow.end());
    }

    void MoveTo(std::wstring& out, int x, int y) {
        out += L"\x1b[" + std::to_wstring(y + 1) + L";" + std::to_wstring(x + 1) + L"H";
    }

    void PrintColored(std::wstring& out, const std::wstring& text, UIColor color) {
        out += L"\x1b[" + ColorCode(color) + L"m" + text + L"\x1b[0m";
    }

    // ---- 박스 그리기 (진짜 유니코드 박스 문자) ----
    void DrawTopBorder(std::wstring& out, int x, int y, int w) {
        MoveTo(out, x, y);
        out += L'┌';
        out += std::wstring(w - 2, L'─');
        out += L'┐';
    }
    void DrawBottomBorder(std::wstring& out, int x, int y, int w) {
        MoveTo(out, x, y);
        out += L'└';
        out += std::wstring(w - 2, L'─');
        out += L'┘';
    }
    void DrawInnerDivider(std::wstring& out, int x, int y, int w) {
        MoveTo(out, x, y);
        out += L'├';
        out += std::wstring(w - 2, L'─');
        out += L'┤';
    }
    void DrawSides(std::wstring& out, int x, int y, int w, int h) {
        for (int row = 1; row < h - 1; ++row) {
            MoveTo(out, x, y + row);
            out += L'│';
            MoveTo(out, x + w - 1, y + row);
            out += L'│';
        }
    }

    void DrawTitle(std::wstring& out, int y, int w, const std::wstring& title) {
        int pad = (w - DisplayWidth(title)) / 2;
        if (pad < 1) pad = 1;
        MoveTo(out, pad, y);
        PrintColored(out, title, UIColor::Cyan);
    }

    // 캐릭터들을 가로로 나란히 배치하고, 세로 방향으로는 영역 중앙에 오도록 그린다.
    void DrawStage(std::wstring& out, int x, int y, int w, int h, const std::vector<ActorDisplay>& actors) {
        if (actors.empty()) return;
        int slotWidth = w / (int)actors.size();
        for (size_t i = 0; i < actors.size(); ++i) {
            int slotX = x + (int)i * slotWidth + 3;

            int contentHeight = (int)actors[i].art.size() + 1 + 1 + (int)actors[i].statLines.size();
            int curY = y + (h - contentHeight) / 2;
            if (curY < y) curY = y;

            for (const auto& artLine : actors[i].art) {
                MoveTo(out, slotX, curY++);
                out += ToWide(artLine);
            }
            curY++;

            MoveTo(out, slotX, curY++);
            PrintColored(out, ToWide(actors[i].name), actors[i].nameColor);

            for (const auto& line : actors[i].statLines) {
                MoveTo(out, slotX, curY++);
                PrintColored(out, ToWide(line.text), line.color);
            }
        }
    }

    // commands 중 [scrollOffset, scrollOffset+visibleRows) 구간만 세로로 나열해서 보여준다.
    void DrawCommandList(std::wstring& out, int x, int y, int visibleRows,
                          const std::vector<MenuOption>& commands, int selectedIndex, int scrollOffset) {
        for (int row = 0; row < visibleRows; ++row) {
            int idx = scrollOffset + row;
            if (idx >= (int)commands.size()) break;

            bool isSelected = (idx == selectedIndex);
            std::wstring label = (isSelected ? std::wstring(L"▶ ") : std::wstring(L"  ")) + ToWide(commands[idx].label);

            MoveTo(out, x + 2, y + row);
            if (isSelected) {
                PrintColored(out, label, UIColor::Yellow);
            }
            else if (commands[idx].color != UIColor::Default) {
                PrintColored(out, label, commands[idx].color);
            }
            else {
                out += label;
            }
        }
    }

    void DrawMultilineText(std::wstring& out, int x, int y, int maxLines, const std::wstring& text) {
        std::wstringstream wss(text);
        std::wstring line;
        int row = 0;
        while (row < maxLines && std::getline(wss, line)) {
            if (!line.empty() && line.back() == L'\r') line.pop_back();
            MoveTo(out, x, y + row);
            out += line;
            row++;
        }
    }

    // ---- 화면 레이아웃 (콘솔 실제 크기에 맞춰 매번 다시 계산) ----
    struct Layout {
        int width;
        int height;
        int stageY;
        int stageH;
        int cmdDividerY;
        int cmdRowY;
        int cmdVisibleRows;
        int logRowY;
        int logLines;
        int footerY;
    };

    Layout ComputeLayout(int commandCount) {
        int cols, rows;
        GetConsoleSize(cols, rows);

        Layout L;
        L.width = (cols > 30) ? cols - 1 : cols;
        L.logLines = 6;

        const int topOverhead = 2;                                   // 제목줄 + 제목 밑 구분선
        const int bottomFixed = 1 + 1 + L.logLines + 1 + 1;          // 구분선 + 여백 + 로그 + 푸터 + 바닥테두리

        int usableHeight = (rows > 15) ? rows - 1 : rows;

        int maxCmdRows = usableHeight - topOverhead - bottomFixed - 5; // 스테이지에 최소 5줄은 남김
        if (maxCmdRows < 1) maxCmdRows = 1;
        if (maxCmdRows > 12) maxCmdRows = 12;

        if (commandCount <= 0) {
            L.cmdVisibleRows = 1;
        }
        else if (commandCount < maxCmdRows) {
            L.cmdVisibleRows = commandCount;
        }
        else {
            L.cmdVisibleRows = maxCmdRows;
        }

        L.stageH = usableHeight - topOverhead - bottomFixed - L.cmdVisibleRows;
        if (L.stageH < 5) L.stageH = 5;

        L.stageY = topOverhead;
        L.cmdDividerY = L.stageY + L.stageH;
        L.cmdRowY = L.cmdDividerY + 1;
        L.logRowY = L.cmdRowY + L.cmdVisibleRows + 1;
        L.footerY = L.logRowY + L.logLines;
        L.height = L.footerY + 2;
        return L;
    }

    void FlushToConsole(const std::wstring& buffer) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written = 0;
        WriteConsoleW(hOut, buffer.c_str(), (DWORD)buffer.size(), &written, nullptr);
    }

    void RenderInternal(const std::string& title,
                         const std::vector<ActorDisplay>& actors,
                         const std::vector<MenuOption>& commands,
                         int selectedIndex,
                         int scrollOffset,
                         const std::string& logMessage,
                         const std::string& footer) {
        Layout L = ComputeLayout((int)commands.size());

        std::wstring out;
        out += L"\x1b[2J";

        DrawTopBorder(out, 0, 0, L.width);
        DrawSides(out, 0, 0, L.width, L.height);
        DrawTitle(out, 0, L.width, ToWide(title));
        DrawInnerDivider(out, 0, 1, L.width);

        DrawStage(out, 0, L.stageY, L.width, L.stageH, actors);

        DrawInnerDivider(out, 0, L.cmdDividerY, L.width);
        DrawCommandList(out, 0, L.cmdRowY, L.cmdVisibleRows, commands, selectedIndex, scrollOffset);

        DrawMultilineText(out, 2, L.logRowY, L.logLines, ToWide(logMessage));

        if (!footer.empty()) {
            MoveTo(out, 2, L.footerY);
            PrintColored(out, ToWide(footer), UIColor::Gray);
        }

        DrawBottomBorder(out, 0, L.height - 1, L.width);
        MoveTo(out, 0, L.height);

        FlushToConsole(out);
    }

} // namespace

void UI_Init() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(hOut, &mode)) {
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

void UI_RenderScreen(const std::string& title,
                      const std::vector<ActorDisplay>& actors,
                      const std::vector<MenuOption>& commands,
                      int selectedIndex,
                      const std::string& logMessage) {
    RenderInternal(title, actors, commands, selectedIndex, 0, logMessage, "");
}

int UI_RunMenuSelector(const std::string& title,
                        const std::vector<ActorDisplay>& actors,
                        const std::vector<MenuOption>& commands,
                        const std::string& logMessage) {
    if (commands.empty()) return -1;

    Layout L = ComputeLayout((int)commands.size());
    int visibleRows = L.cmdVisibleRows;
    int count = (int)commands.size();

    int selected = 0;
    int scrollOffset = 0;

    auto MakeFooter = [&]() {
        std::string footer = "↑↓ 이동   Enter 선택   Esc 뒤로";
        if (count > visibleRows) {
            footer += "   (" + std::to_string(selected + 1) + "/" + std::to_string(count) + ")";
        }
        return footer;
    };

    RenderInternal(title, actors, commands, selected, scrollOffset, logMessage, MakeFooter());

    while (true) {
        int key = _getch();

        if (key == 224 || key == 0) {              // 방향키는 두 번에 나눠서 들어옴
            int key2 = _getch();
            if (key2 == 72) {                       // ↑
                selected = (selected - 1 + count) % count;
            }
            else if (key2 == 80) {                  // ↓
                selected = (selected + 1) % count;
            }

            if (selected == 0) {
                scrollOffset = 0;
            }
            else if (selected == count - 1) {
                scrollOffset = (count - visibleRows > 0) ? (count - visibleRows) : 0;
            }
            else if (selected < scrollOffset) {
                scrollOffset = selected;
            }
            else if (selected >= scrollOffset + visibleRows) {
                scrollOffset = selected - visibleRows + 1;
            }

            RenderInternal(title, actors, commands, selected, scrollOffset, logMessage, MakeFooter());
        }
        else if (key == 13) {                       // Enter
            return selected;
        }
        else if (key == 27) {                       // Esc
            return -1;
        }
    }
}

void UI_ShowMessage(const std::string& title,
                     const std::vector<ActorDisplay>& actors,
                     const std::string& message) {
    std::vector<MenuOption> noCommands;
    RenderInternal(title, actors, noCommands, -1, 0, message, "Enter 계속");

    while (_getch() != 13) {
        // Enter(13)가 나올 때까지 다른 입력은 무시
    }
}

void UI_ShowStory(const std::string& title,
                   const std::vector<UILine>& lines) {
    Layout L = ComputeLayout(0);

    std::wstring out;
    out += L"\x1b[2J";

    DrawTopBorder(out, 0, 0, L.width);
    DrawSides(out, 0, 0, L.width, L.height);
    DrawTitle(out, 0, L.width, ToWide(title));
    DrawInnerDivider(out, 0, 1, L.width);
    DrawBottomBorder(out, 0, L.height - 1, L.width);

    // 본문 줄들을 스테이지+커맨드+로그 영역 전체에 걸쳐 세로 중앙 정렬
    int areaTop = L.stageY;
    int areaHeight = L.footerY - L.stageY;
    int startY = areaTop + (areaHeight - (int)lines.size()) / 2;
    if (startY < areaTop) startY = areaTop;

    int y = startY;
    for (const auto& line : lines) {
        if (y >= L.footerY) break;
        MoveTo(out, 6, y++);
        PrintColored(out, ToWide(line.text), line.color);
    }

    MoveTo(out, 2, L.footerY);
    PrintColored(out, ToWide(std::string("Enter 계속")), UIColor::Gray);

    FlushToConsole(out);

    while (_getch() != 13) {
        // Enter만 허용
    }
}

void UI_ShowInputScreen(const std::string& title,
                         const std::vector<ActorDisplay>& actors,
                         const std::string& promptText) {
    Layout L = ComputeLayout(0);

    std::wstring out;
    out += L"\x1b[2J";

    DrawTopBorder(out, 0, 0, L.width);
    DrawSides(out, 0, 0, L.width, L.height);
    DrawTitle(out, 0, L.width, ToWide(title));
    DrawInnerDivider(out, 0, 1, L.width);

    DrawStage(out, 0, L.stageY, L.width, L.stageH, actors);

    DrawInnerDivider(out, 0, L.cmdDividerY, L.width);
    DrawBottomBorder(out, 0, L.height - 1, L.width);

    // 프롬프트를 로그 영역에 띄우고, 그 바로 뒤에 커서를 남겨서
    // 이어지는 std::cin 입력이 박스 안에 자연스럽게 표시되게 한다.
    MoveTo(out, 2, L.logRowY);
    out += ToWide(promptText);

    FlushToConsole(out);
}

void UI_ShowSplash(const std::wstring& art) {
    std::wstring out;
    out += L"\x1b[2J";

    std::wstringstream wss(art);
    std::wstring line;
    int row = 0;
    while (std::getline(wss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        MoveTo(out, 0, row++);
        out += line;
    }

    FlushToConsole(out);
    _getch(); // 아무 키나 누르면 진행
}

std::string UI_Colored(const std::string& text, UIColor color) {
    return std::string("\x1b[") + ColorCodeNarrow(color) + "m" + text + "\x1b[0m";
}

std::string UI_MakeGauge(int current, int max, int slots) {
    if (max <= 0) max = 1;
    if (current < 0) current = 0;
    if (current > max) current = max;

    // 체력이 1이라도 남아 있으면 최소 1칸은 채워 보이게 올림 처리
    int filled = (current * slots + max - 1) / max;
    if (filled > slots) filled = slots;

    std::string bar;
    for (int i = 0; i < slots; ++i) {
        bar += (i < filled) ? "■" : "□";
    }
    return bar + " " + std::to_string(current) + "/" + std::to_string(max);
}
