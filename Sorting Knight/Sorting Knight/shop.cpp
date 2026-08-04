#define NOMINMAX
#include "shop.h"
#include "ItemManager.h"
#include "inventory.h"
#include "gameText.h"
#include "ui.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <windows.h>

Shop::Shop(std::vector<Item> stock) : stock(std::move(stock)) {}

BuyResult Shop::Buy(Player& player, const std::string& itemName, int count) {
    const Item* item = FindInStock(itemName);
    if (!item) { return BuyResult::NOT_FOUND; }
    int totalPrice = item->GetPrice() * count;
    if (player.GetGold() < totalPrice) { return BuyResult::NOT_ENOUGH_GOLD; }
    if (player.GetInventory().GetRemainingSpace() < count) { return BuyResult::NOT_ENOUGH_SPACE; }
    player.SpendGold(totalPrice);
    player.GetInventory().AddItem(*item, count);
    return BuyResult::SUCCESS;
}

bool Shop::Sell(Player& player, const std::string& itemName, int count) {
    if (!player.GetInventory().HasItem(itemName, count)) { return false; }
    const Item* item = player.GetInventory().FindItem(itemName);
    if (!item) { return false; }
    int sellPrice = static_cast<int>(std::round(item->GetPrice() * SELL_RATE)) * count;
    player.GetInventory().RemoveItem(itemName, count);
    player.AddGold(sellPrice);
    return true;
}

const std::vector<Item>& Shop::GetStock() const {
    return stock;
}

void Shop::ShowStock() const {
    // [사용 안 함] UI는 EnterShopMenu 쪽에서 처리
}

const Item* Shop::FindInStock(const std::string& itemName) const {
    for (const auto& item : stock) {
        if (item.GetName() == itemName) { return &item; }
    }
    return nullptr;
}


namespace {

    Player* g_player = nullptr;
    ItemManager* g_itemManager = nullptr;

    std::string GetShopRarityColor(ItemRarity r) {
        switch (r) {
        case ItemRarity::C: return "37"; // 흰색 (일반)
        case ItemRarity::B: return "94"; // 밝은 파랑 (레어)
        case ItemRarity::A: return "95"; // 밝은 보라 (에픽)
        case ItemRarity::S: return "93"; // 금색 (전설)
        default: return "37";
        }
    }

    // 상점에서는 C등급(최하 등급)만 판매. B/A/S 등급은 "등급 합성"으로만 획득 가능.
    const std::vector<std::string> weaponItemNames = {
        "평범한 가위", "작업용 쇠지렛대", "대나무 빗자루",
        "분리수거 집게", "두꺼운 고무장갑"
    };

    const std::vector<std::string> consumableItemNames = {
        "박카스", "응급 구급상자", "아드레날린 앰플", "전투 각성제",
        "특제 단백질 보충제", "생명력의 영약", "불법 개조 부품",
        "장인의 숫돌", "환경부 장관의 표창장", "기적의 엘릭서"
    };

    std::vector<Item> BuildStock(const ItemManager& itemManager, const std::vector<std::string>& names) {
        std::vector<Item> result;
        result.reserve(names.size());
        for (const auto& name : names) {
            result.push_back(itemManager.GetItem(name));
        }
        return result;
    }

    // 새로 만드는 함수: 상점 화면에 보여줄 상인 아스키 아트
    std::vector<std::string> GetShopKeeperArt() {
        return {
          "   /\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\              ",
            "        |                                  |        ",
            "        |            [ S H O P ]           |        ",
            "        |                                  |        ",
            "       [i]                                \\%/      ",
            "        |   ~\\|/~~~~~~\\|/~~~~~~\\|/~~~~  |        ",
            "        |                                  |        ",
            "        |           \\(^ ▽ ^ )            |        ",
            "        |            |      |\\            |       ",
            "        |            |______| \\           |       ",
            "     ========================================     ",
            "      |  (8)(8)   [|||||]   (8)(8)    ($)  |      ",
            "      |  (8)(8)   [|||||]   (8)(8)         |      ",
            "      |                                    |      ",
            "      |====================================|      ",
            "      |      |                      |      |      ",
            "      |      |                      |      |      ",
            "      ======================================      ",
        };
    }

    // 새로 만드는 함수: 장비 보급 화면에 보여줄 아트
    std::vector<std::string> GetWeaponMerchantArt() {
        return {
            "   /\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\",
            "        |                                |        ",
            "        |          [ S H O P ]           |        ",
            "        |                                |        ",
            "      [i]                                \\%/      ",
            "        |   ~\\|/~~~~~~\\|/~~~~~~\\|/~~~~   |        ",
            "        |                                |        ",
            "        |   8<      /====--    //|||||   |        ", // 가위, 쇠지렛대, 빗자루
            "        |  / |      |         //  |||    |        ",
            "        |  \\\\ /      |  .==.  //    |     |        ", // 집게, 고무장갑
            "     ========================================     ",
            "      |  (8)(8)   [|||||]   (8)(8)    ($)  |      ",
            "      |  (8)(8)   [|||||]   (8)(8)         |      ",
            "      |                                    |      ",
            "      |====================================|      ",
            "      |      |                      |      |      ",
            "      |      |                      |      |      ",
            "      ======================================      ",
        };
    }


    // 새로 만드는 함수: 소모품 상점
    std::vector<std::string> GetPotionMerchantArt() {
        return {
            "   /\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\",
            "        |          [ S H O P ]           |        ",
            "      [i]                                \\%/      ",
            "        |   ~\\|/~~~~~~\\|/~~~~~~\\|/~~~~   |        ",
            "        |     .-.         _        ====  |        ",
            "        |    | U |       (_)       |  |  |        ",
            "        |    |___|      ( + )      '=='  |        ",
            "     ========================================     ",
            "      |  (8)(8)   [|||||]   (8)(8)    ($)  |      ",
            "      |  (8)(8)   [|||||]   (8)(8)         |      ",
            "      |====================================|      ",
            "      |      |                      |      |      ",
            "      |      |                      |      |      ",
            "      ======================================      ",
        };
    }


    // 새로 만드는 구조체: 상황별 대사 묶음
    struct ShopDialogue {
        std::string entry;
        std::string buySuccess;
        std::string notEnoughGold;
        std::string notEnoughSpace;
    };

    // 새로 만드는 변수: 장비 상점 대사
    const ShopDialogue weaponShopDialogue = {
        "\"그놈들을 상대하려면, 보통 물건으로는 힘들다네.\"",
        "\"좋은 선택이야. 원 주인은 몰랐던 가치지.\"",
        "\"외상은 사절이야. 나라도 망했는데.\"",
        "\"자네도 슬슬 버리는 법을 배워야지.\""
    };

    
    // 새로 만드는 변수: 포션 상점 대사
    const ShopDialogue potionShopDialogue = {
        "\"마실 땐 아끼지 말고 마셔. 목숨은 1개라네.\"",
        "\"좋은 선택이야. 원 주인은 몰랐던 가치지.\"",
        "\"외상은 사절이야. 나라도 망했는데.\"",
        "\"자네도 슬슬 버리는 법을 배워야지.\""
    };



    // 새로 만드는 함수 : 아트 전체를 박스 가운데로 오도록 왼쪽에 공백을 붙여줌
        std::vector<std::string> CenterArt(const std::vector<std::string>& art, int contentWidth) {
        int maxWidth = 0;
        for (const auto& line : art) {
            int w = DisplayWidth(line);
            if (w > maxWidth) { maxWidth = w; }
        }
        int leftPad = (contentWidth - maxWidth) / 2;
        if (leftPad < 0) { leftPad = 0; }

        std::vector<std::string> result;
        for (const auto& line : art) {
            result.push_back(std::string(leftPad, ' ') + line);
        }
        return result;
    }

        // 새로 만드는 함수: 아트를 오른쪽 끝으로 밀고, 왼쪽엔 대사 한 줄을 세로 중간쯤에 넣어줌
        std::vector<std::string> MakeArtWithDialogue(const std::vector<std::string>& art, const std::string& dialogue, int contentWidth) {
            int artWidth = 0;
            for (const auto& line : art) {
                int w = DisplayWidth(line);
                if (w > artWidth) { artWidth = w; }
            }
            int leftGap = contentWidth - artWidth;   // 아트 왼쪽에 남는 공간 (대사 넣을 자리)
            if (leftGap < 0) { leftGap = 0; }

            const int textLeftMargin = 4;
            int dialogueRow = (int)art.size() / 2;   // 대사를 넣을 줄 (아트 세로 중간)

            std::vector<std::string> result;
            for (size_t i = 0; i < art.size(); ++i) {
                std::string prefix;
                if ((int)i == dialogueRow) {
                    std::string indented = std::string(textLeftMargin, ' ') + dialogue;
                    int textWidth = DisplayWidth(indented);
                    int pad = leftGap - textWidth;
                    if (pad < 0) { pad = 0; }
                    prefix = indented + std::string(pad, ' ');
                }
                else {
                    prefix = std::string(leftGap, ' ');   // ← 새로 추가된 부분: 대사가 없는 줄은 이만큼 공백을 채워서 아트를 오른쪽으로 밀어줌
                }
                result.push_back(prefix + art[i]);
            }
            return result;
        }

    void ClearCinError() {
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
    }

    std::string BuyResultText(BuyResult result, const std::string& itemName) {
        switch (result) {
        case BuyResult::SUCCESS:
            return Color("[구매 완료] " + itemName, "92");
        case BuyResult::NOT_FOUND:
            return Color("[!] 존재하지 않는 물품입니다.", "91");
        case BuyResult::NOT_ENOUGH_GOLD:
            return Color("[!] 예산이 부족합니다.", "91");
        case BuyResult::NOT_ENOUGH_SPACE:
            return Color("[!] 적재 공간이 부족합니다.", "91");
        }
        return "";
    }

    // ── 구매 화면 ──────────────────────────────────────────
    void BuyMenu(Player* player, Shop& shop, const std::string& title, const std::vector<std::string>& shopArt, const ShopDialogue& dialogue) {
        std::string notice = "";
        std::string currentLine = dialogue.entry;   // 새로 추가: 지금 보여줄 대사 (처음엔 입장 대사)
        while (true) {
            const auto& stock = shop.GetStock();
            std::vector<std::string> body = { "" };
            if (stock.empty()) {
                body.push_back(Color("  (재고 없음)", "90"));
            }
            else {
                for (size_t i = 0; i < stock.size(); ++i) {
                    const Item& item = stock[i];
                    std::string line = "  " + std::to_string(i + 1) + ". ";
                    if (item.GetCategory() == ItemCategory::WEAPON) {
                        std::string nameAndRarity = item.GetName() + " [" + item.GetRarityString() + "등급]";
                        line += Color(nameAndRarity, GetShopRarityColor(item.GetRarity()).c_str()) + " "
                            + Color(item.GetPropertyString(), "90") + "   " + Color(std::to_string(item.GetPrice()) + "G", "93");
                    }
                    else {
                        line += item.GetName() + " " + Color(item.GetEffectString(), "92") + "   " + Color(std::to_string(item.GetPrice()) + "G", "93");
                    }
                    body.push_back(line);
                }
            }
            body.push_back("");
            body.push_back("  0. 이전 메뉴로");
            body.push_back("");
            std::vector<std::string> art = MakeArtWithDialogue(shopArt, currentLine, 114);   // 새로 추가: 대사 얹은 아트
            art.insert(art.begin(), 1, "");   // 아트 위에 빈 줄 1개
            std::vector<std::string> footer = {
                "예산 " + Color(std::to_string(player->GetGold()) + "G", "93")
                + "      적재 " + std::to_string(player->GetInventory().GetTotalItemCount())
                + "/" + std::to_string(Inventory::MAX_ITEM_COUNT),
                notice,
                "물품 번호: "
            };
            DrawScreen(Color(title, "93"), body, art, footer);
            std::cout << "\033[37;15H";
            notice = "";
            int choice;
            if (!(std::cin >> choice)) {
                ClearCinError();
                notice = Color("[!] 잘못된 입력입니다.", "91");
                continue;
            }
            ClearCinError();
            if (choice == 0) { break; }
            int index = choice - 1;
            if (index < 0 || index >= (int)stock.size()) {
                notice = Color("[!] 잘못된 선택입니다.", "91");
                continue;
            }
            const std::string itemName = stock[index].GetName();
            std::vector<std::string> qBody = {
                "",
                "  " + itemName,
                "  단가 " + Color(std::to_string(stock[index].GetPrice()) + "G", "93"),
                "",
                Color("  ※ 수량을 입력하십시오.", "90"),
                ""
            };
            std::vector<std::string> qFooter = {
                "예산 " + Color(std::to_string(player->GetGold()) + "G", "93"),
                "",
                "수량: "
            };
            DrawScreen(Color("구매 신청", "93"), qBody, art, qFooter);
            std::cout << "\033[37;15H";
            int count;
            if (!(std::cin >> count)) {
                ClearCinError();
                notice = Color("[!] 잘못된 입력입니다.", "91");
                continue;
            }
            ClearCinError();
            if (count <= 0) {
                notice = Color("[!] 1개 이상 입력하십시오.", "91");
                continue;
            }
            BuyResult result = shop.Buy(*player, itemName, count);
            notice = BuyResultText(result, itemName);

            // 새로 추가: 구매 결과에 따라 대사도 같이 바꿔줌
            switch (result) {
            case BuyResult::SUCCESS: currentLine = dialogue.buySuccess; break;
            case BuyResult::NOT_ENOUGH_GOLD: currentLine = dialogue.notEnoughGold; break;
            case BuyResult::NOT_ENOUGH_SPACE: currentLine = dialogue.notEnoughSpace; break;
            default: break;
            }
        }
    }

    // ── 합성기(가챠 머신) 아트 ─────────────────────────────
    // 화면 좌표를 정확히 알 수 있도록 모든 폭을 상수로 고정해서 만든다.
    const int GACHA_INNER_W = 47;   // 머신 내부 폭 (표시 폭 기준)
    const int GACHA_LEFT_PAD = 32;  // 머신 왼쪽 여백 (117폭 박스 기준 중앙 배치)
    const int GACHA_REEL_W = 35;    // 릴 창 내부 폭

    // 표시 폭 기준으로 문자열을 가운데 정렬 (색상 코드는 폭 계산에서 제외됨)
    std::string GachaCenter(const std::string& s, int width) {
        int w = DisplayWidth(s);
        int left = (width - w) / 2;
        if (left < 0) { left = 0; }
        int right = width - w - left;
        if (right < 0) { right = 0; }
        return std::string(left, ' ') + s + std::string(right, ' ');
    }

    std::string GachaRow(const std::string& inner) {
        return std::string(GACHA_LEFT_PAD, ' ') + "|" + GachaCenter(inner, GACHA_INNER_W) + "|";
    }

    std::string GachaEdge(char fill) {
        return std::string(GACHA_LEFT_PAD, ' ') + "+" + std::string(GACHA_INNER_W, fill) + "+";
    }

    // reelText: 릴 창(가운데 표시창)에 넣을 문자열
    // ※ 릴 창은 machine[6]번째 줄. 애니메이션 좌표 상수와 반드시 같이 맞춰야 함.
    std::vector<std::string> BuildSynthesizerArt(const std::string& reelText) {
        std::string reelTop = "." + std::string(GACHA_REEL_W + 2, '-') + ".";
        std::string reelLine = "| " + GachaCenter(reelText, GACHA_REEL_W) + " |";
        std::string reelBottom = "'" + std::string(GACHA_REEL_W + 2, '-') + "'";

        std::string recipe = Color("[C]x4", "37") + " -> " + Color("[B]", "94") + "     "
            + Color("[B]x3", "94") + " -> " + Color("[A]", "95") + "     "
            + Color("[A]x2", "95") + " -> " + Color("[S]", "93");

        return {
            GachaEdge('='),                                                  // [0]
            GachaRow(Color("<< 등 급 합 성 기  MK-777 >>", "96")),           // [1]
            GachaRow(Color("\"인생은 한 방, 분리수거도 한 방\"", "90")),     // [2]
            GachaEdge('='),                                                  // [3]
            GachaRow("(*)   (*)   (*)   (*)   (*)   (*)"),                   // [4] 램프 (애니메이션)
            GachaRow(reelTop),                                               // [5]
            GachaRow(reelLine),                                              // [6] 릴 창 (애니메이션)
            GachaRow(reelBottom),                                            // [7]
            GachaRow(""),                                                    // [8]
            GachaRow(recipe),                                                // [9]
            GachaEdge('-'),                                                  // [10]
            GachaRow("IN: 재료 투입구          OUT: 결과 배출구"),           // [11]
            GachaRow("[=========]              [=========]"),                // [12]
            GachaEdge('='),                                                  // [13]
        };
    }

    // ── 가챠 연출 ──────────────────────────────────────────
    // 합성 확정 후 호출. 릴 스핀(감속) → 긴장 → 결과 깜빡임 공개 → Enter 대기.
    void PlayGachaAnimation(const Item& resultItem, ItemManager& itemManager) {
        // 전용 화면: 머신 아트를 body에 고정 배치해서 릴/램프의 콘솔 좌표를 확정한다.
        std::vector<std::string> body = { "" };
        std::vector<std::string> machine = BuildSynthesizerArt("");
        for (const auto& line : machine) { body.push_back(line); }
        body.push_back("");

        std::vector<std::string> art = {};
        std::vector<std::string> footer = { "", Color("합성기가 가동 중입니다... 손대지 마시오.", "90"), "" };
        DrawScreen(Color("등급 합성 - 가동 중", "95"), body, art, footer);

        // 콘솔 좌표 (DrawScreen 구조 기준: body는 4행부터 시작)
        const int lampRow = 4 + 1 + 4;                              // body[5] = machine[4]
        const int reelRow = 4 + 1 + 6;                              // body[7] = machine[6]
        const int lampCol = 3 + GACHA_LEFT_PAD + 1;                 // 머신 내부 시작 열
        const int reelCol = 3 + GACHA_LEFT_PAD + 1 + 4 + 2;         // 릴 창 내용 시작 열

        std::cout << "\033[?25l"; // 커서 숨김

        // 1) 릴 스핀: 무작위 무기 이름이 점점 느려지며 지나감
        ItemRarity spinPool[] = { ItemRarity::C, ItemRarity::B, ItemRarity::A, ItemRarity::S };
        const int totalFrames = 22;
        for (int i = 0; i < totalFrames; ++i) {
            Item spinItem = itemManager.GetRandomItemByRarity(spinPool[rand() % 4]);
            std::string spinColor = GetShopRarityColor(spinItem.GetRarity());
            std::cout << "\033[" << reelRow << ";" << reelCol << "H"
                << Color(GachaCenter(spinItem.GetName(), GACHA_REEL_W), spinColor.c_str());

            std::string lampColor = (i % 2 == 0) ? "93" : "95";
            std::cout << "\033[" << lampRow << ";" << lampCol << "H"
                << Color(GachaCenter("(*)   (*)   (*)   (*)   (*)   (*)", GACHA_INNER_W), lampColor.c_str());

            std::cout << std::flush;
            Sleep(35 + i * i / 3); // 점점 감속
        }

        // 2) 긴장 연출: ? ? ? 깜빡임
        for (int i = 0; i < 3; ++i) {
            std::cout << "\033[" << reelRow << ";" << reelCol << "H"
                << Color(GachaCenter("? ? ?", GACHA_REEL_W), "97") << std::flush;
            Sleep(230);
            std::cout << "\033[" << reelRow << ";" << reelCol << "H"
                << GachaCenter("", GACHA_REEL_W) << std::flush;
            Sleep(120);
        }

        // 3) 결과 공개: 등급 색으로 깜빡이며 등장
        std::string rColor = GetShopRarityColor(resultItem.GetRarity());
        std::string resultText = resultItem.GetName() + " [" + resultItem.GetRarityString() + "]";
        for (int i = 0; i < 5; ++i) {
            std::cout << "\033[" << reelRow << ";" << reelCol << "H"
                << Color(GachaCenter(resultText, GACHA_REEL_W), (i % 2 == 0) ? "97" : rColor.c_str())
                << std::flush;
            Sleep(150);
        }
        std::cout << "\033[" << lampRow << ";" << lampCol << "H"
            << Color(GachaCenter("(o)   (o)   (o)   (o)   (o)   (o)", GACHA_INNER_W), rColor.c_str());
        std::cout << "\033[" << reelRow << ";" << reelCol << "H"
            << Color(GachaCenter(resultText, GACHA_REEL_W), rColor.c_str()) << std::flush;

        // 4) 등급별 축하 문구 (박스 안 여백 줄에 타이핑)
        std::string burst;
        if (resultItem.GetRarity() == ItemRarity::S) { burst = "!!! 잭 팟 !!!  전설의 S등급 강림 !!!"; }
        else if (resultItem.GetRarity() == ItemRarity::A) { burst = "!! 대박 !!  A등급 획득 !!"; }
        else { burst = "합성 성공! " + resultItem.GetRarityString() + "등급 획득!"; }
        int burstCol = 3 + (114 - DisplayWidth(burst)) / 2;
        TypeTextAt(22, burstCol, burst, rColor, 25);

        // 5) 안내 문구 교체 후 Enter 대기
        std::cout << "\033[36;3H" << GachaCenter(Color("[ Enter: 결과 회수 ]", "37"), 114) << std::flush;
        std::cout << "\033[?25h" << "\033[0m"; // 커서 복구
        std::cin.ignore();
        std::cin.get();
    }

    // ── 등급 합성 ──────────────────────────────────────────
    // 같은 등급의 무기 5개(종류 무관)를 소모해 다음 등급 무기 1개를 무작위로 획득.
    // 등급 순서: C(최하) -> B -> A -> S(최고)
    int GetUpgradeRequirement(ItemRarity rarity) {
        switch (rarity) {
        case ItemRarity::C: return 4; // C -> B: 4개 필요
        case ItemRarity::B: return 3; // B -> A: 3개 필요
        case ItemRarity::A: return 2; // A -> S: 2개 필요
        default: return 999;
        }
    }

    // 보유한 무기 중 해당 등급인 것들의 개수 합 (종류 무관)
    int CountByRarity(Player* player, ItemRarity rarity) {
        int total = 0;
        for (const auto& slot : player->GetInventory().GetItems()) {
            if (slot.first.GetCategory() == ItemCategory::WEAPON && slot.first.GetRarity() == rarity) {
                total += slot.second;
            }
        }
        return total;
    }

    // 해당 등급 무기를 종류 무관하게 총 count개 제거.
    bool ConsumeByRarity(Player* player, ItemRarity rarity, int count) {
        if (CountByRarity(player, rarity) < count) {
            return false;
        }

        std::vector<std::pair<std::string, int>> targets;
        for (const auto& slot : player->GetInventory().GetItems()) {
            if (slot.first.GetCategory() == ItemCategory::WEAPON && slot.first.GetRarity() == rarity) {
                targets.push_back({ slot.first.GetName(), slot.second });
            }
        }

        int remaining = count;
        for (const auto& target : targets) {
            if (remaining <= 0) { break; }
            int take = std::min(remaining, target.second);
            player->GetInventory().RemoveItem(target.first, take);
            remaining -= take;
        }
        return true;
    }

    void RarityUpgradeMenu(Player* player, ItemManager& itemManager) {
        static const std::vector<std::pair<ItemRarity, ItemRarity>> steps = {
            { ItemRarity::C, ItemRarity::B },
            { ItemRarity::B, ItemRarity::A },
            { ItemRarity::A, ItemRarity::S }
        };

        // ★ [등급] 괄호 전체에 색을 입히는 함수로 교체 완료!
        auto coloredBracket = [](ItemRarity r) -> std::string {
            switch (r) {
            case ItemRarity::C: return Color("[C등급]", "37");
            case ItemRarity::B: return Color("[B등급]", "94");
            case ItemRarity::A: return Color("[A등급]", "95");
            case ItemRarity::S: return Color("[S등급]", "93");
            default: return "[?등급]";
            }
            };

        std::string notice = "";

        while (true) {
            std::vector<std::string> body = { "" };
            for (size_t i = 0; i < steps.size(); ++i) {
                ItemRarity from = steps[i].first;
                ItemRarity to = steps[i].second;

                int have = CountByRarity(player, from);
                int reqCount = GetUpgradeRequirement(from);
                bool canUpgrade = have >= reqCount;

                // ★ 합성 경로 출력: [C등급] -> [B등급] 형태로 변경
                std::string line = "  " + std::to_string(i + 1) + ". "
                    + coloredBracket(from) + " -> "
                    + coloredBracket(to) + "    재료 "
                    + std::to_string(have) + "/" + std::to_string(reqCount);
                line += canUpgrade ? Color("   [가능]", "92") : Color("   [재료 부족]", "90");
                body.push_back(line);
            }
            body.push_back("");
            body.push_back("  0. 이전 메뉴로");
            body.push_back("");

            std::vector<std::string> footer = {
                Color("※ 지정된 개수의 하위 등급 무기를 소모해 상위 등급 무기를 무작위로 얻습니다.", "90"),
                notice,
                "선택: "
            };

            std::vector<std::string> art = BuildSynthesizerArt(Color("재료를 넣고 합성 번호를 입력하시오", "90"));
            art.insert(art.begin(), 1, "");
            DrawScreen(Color("등급 합성", "95"), body, art, footer);
            std::cout << "\033[37;11H";
            notice = "";

            int choice;
            if (!(std::cin >> choice)) {
                ClearCinError();
                notice = Color("[!] 잘못된 입력입니다.", "91");
                continue;
            }
            ClearCinError();

            if (choice == 0) { break; }

            int index = choice - 1;
            if (index < 0 || index >= (int)steps.size()) {
                notice = Color("[!] 잘못된 선택입니다.", "91");
                continue;
            }

            ItemRarity from = steps[index].first;
            ItemRarity to = steps[index].second;
            int reqCount = GetUpgradeRequirement(from);

            if (!ConsumeByRarity(player, from, reqCount)) {
                // ★ 재료 부족 에러 메시지 괄호 찐빠 해결 완료!
                notice = Color("[!] 재료가 부족합니다. (", "91")
                    + coloredBracket(from)
                    + Color(" " + std::to_string(reqCount) + "개 필요)", "91");
                continue;
            }

            Item resultItem = itemManager.GetRandomItemByRarity(to);
            player->GetInventory().AddItem(resultItem, 1);
            PlayGachaAnimation(resultItem, itemManager);   // ★ 가챠 연출
            notice = Color("[합성 완료] " + resultItem.GetName() + " 획득!", "92");
        }
    }

} // namespace


void InitShop(Player* player, ItemManager* itemManager) {
    g_player = player;
    g_itemManager = itemManager;
}

void EnterShopMenu() {
    if (!g_player || !g_itemManager) { return; }

    Player* player = g_player;
    ItemManager& itemManager = *g_itemManager;

    Shop weaponShop(BuildStock(itemManager, weaponItemNames));
    Shop consumableShop(BuildStock(itemManager, consumableItemNames));

    std::string notice = "";
    bool inShop = true;

    while (inShop) {
        std::vector<std::string> body = {
            "",
            "  " + Color("1", "96") + ". 장비 수령",
            "  " + Color("2", "96") + ". 소모품 수령",
            "  " + Color("3", "96") + ". 등급 합성",
            Color("  0. 나가기", "90"),
            ""
        };
        std::vector<std::string> footer = {
            "예산 " + Color(std::to_string(player->GetGold()) + "G", "93"),
            notice,
            "선택: "
        };

        std::vector<std::string> art = MakeArtWithDialogue(GetShopKeeperArt(), "\"어서 오게. 버려진 것에도 값은 있지.\"", 114);
        art.insert(art.begin(), 3, "");


        DrawScreen(Color("보급소", "93"), body, art, footer);
        std::cout << "\033[37;11H";
        notice = "";

        int choice;
        if (!(std::cin >> choice)) {
            ClearCinError();
            notice = Color("[!] 잘못된 입력입니다.", "91");
            continue;
        }
        ClearCinError();

        switch (choice) {
        case 1: BuyMenu(player, weaponShop, "장비 보급", GetWeaponMerchantArt(), weaponShopDialogue); break;
        case 2: BuyMenu(player, consumableShop, "소모품 보급", GetPotionMerchantArt(), potionShopDialogue); break;
        case 3: RarityUpgradeMenu(player, itemManager); break;
        case 0: inShop = false; break;
        default: notice = Color("[!] 잘못된 선택입니다.", "91"); break;
        }
    }
}