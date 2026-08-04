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
        case ItemRarity::C: return "37";
        case ItemRarity::B: return "93";
        case ItemRarity::A: return "94";
        case ItemRarity::S: return "95";
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
                            + Color(item.GetPropertyString(), "90") + "   " + std::to_string(item.GetPrice()) + "G";
                    }
                    else {
                        line += item.GetName() + " " + Color(item.GetEffectString(), "92") + "   " + std::to_string(item.GetPrice()) + "G";
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
            DrawScreen(title, body, art, footer);
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
                "  단가 " + std::to_string(stock[index].GetPrice()) + "G",
                "",
                Color("  ※ 수량을 입력하십시오.", "90"),
                ""
            };
            std::vector<std::string> qFooter = {
                "예산 " + Color(std::to_string(player->GetGold()) + "G", "93"),
                "",
                "수량: "
            };
            DrawScreen("구매 신청", qBody, art, qFooter);
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
            case ItemRarity::B: return Color("[B등급]", "93");
            case ItemRarity::A: return Color("[A등급]", "94");
            case ItemRarity::S: return Color("[S등급]", "95");
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

            std::vector<std::string> art = {
                "",
                Color(R"(        [  보 급 소  ] )", "96"),
                Color(R"(       ====╦════════ )", "90"),
                Color(R"(         _//_        )", "90"),
                Color(R"(        [____]       )", "90"),
                ""
            };
            DrawScreen("등급 합성", body, art, footer);
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
            "  1. 장비 수령",
            "  2. 소모품 수령",
            "  3. 등급 합성",
            "  0. 나가기",
            ""
        };
        std::vector<std::string> footer = {
            "예산 " + Color(std::to_string(player->GetGold()) + "G", "93"),
            notice,
            "선택: "
        };

        std::vector<std::string> art = MakeArtWithDialogue(GetShopKeeperArt(), "\"어서 오게. 버려진 것에도 값은 있지.\"", 114);
        art.insert(art.begin(), 3, "");


        DrawScreen("보급소", body, art, footer);
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