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

    // 상점에서는 C등급(최하 등급)만 판매. B/A/S 등급은 "등급 합성"으로만 획득 가능.
    const std::vector<std::string> weaponItemNames = {
        "평범한 가위", "나무 젓가락", "대나무 빗자루",
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
    void BuyMenu(Player* player, Shop& shop, const std::string& title) {
        std::string notice = "";

        while (true) {
            const auto& stock = shop.GetStock();

            std::vector<std::string> body = { "" };
            if (stock.empty()) {
                body.push_back(Color("  (재고 없음)", "90"));
            }
            else {
                for (size_t i = 0; i < stock.size(); ++i) {
                    const Item& item = stock[i];
                    body.push_back("  " + std::to_string(i + 1) + ". " + item.GetName()
                        + "   [" + item.GetRarityString() + "등급]   "
                        + std::to_string(item.GetPrice()) + "G");
                }
            }
            body.push_back("");
            body.push_back("  0. 이전 메뉴로");
            body.push_back("");

            std::vector<std::string> footer = {
                "예산 " + Color(std::to_string(player->GetGold()) + "G", "93")
                + "      적재 " + std::to_string(player->GetInventory().GetTotalItemCount())
                + "/" + std::to_string(Inventory::MAX_ITEM_COUNT),
                notice,
                "물품 번호: "
            };

            DrawScreen(title, body, footer);
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
            DrawScreen("구매 신청", qBody, qFooter);
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
        }
    }

    // ── 등급 합성 ──────────────────────────────────────────
    // 같은 등급의 무기 5개(종류 무관)를 소모해 다음 등급 무기 1개를 무작위로 획득.
    // 등급 순서: C(최하) -> B -> A -> S(최고)
    const int UPGRADE_REQUIRE_COUNT = 5;

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
    // GetItems()는 내부 벡터의 참조라 순회 중 RemoveItem을 부르면 벡터가 변형됨
    // -> 먼저 (이름, 개수) 스냅샷을 뜬 뒤 그 기준으로 제거.
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

        auto rarityChar = [](ItemRarity r) -> char {
            switch (r) {
            case ItemRarity::C: return 'C';
            case ItemRarity::B: return 'B';
            case ItemRarity::A: return 'A';
            case ItemRarity::S: return 'S';
            default: return '?';
            }
            };

        std::string notice = "";

        while (true) {
            std::vector<std::string> body = { "" };
            for (size_t i = 0; i < steps.size(); ++i) {
                ItemRarity from = steps[i].first;
                ItemRarity to = steps[i].second;
                int have = CountByRarity(player, from);
                bool canUpgrade = have >= UPGRADE_REQUIRE_COUNT;

                std::string line = "  " + std::to_string(i + 1) + ". ["
                    + std::string(1, rarityChar(from)) + "등급 -> "
                    + std::string(1, rarityChar(to)) + "등급]    재료 "
                    + std::to_string(have) + "/" + std::to_string(UPGRADE_REQUIRE_COUNT);
                line += canUpgrade ? Color("   [가능]", "92") : Color("   [재료 부족]", "90");
                body.push_back(line);
            }
            body.push_back("");
            body.push_back("  0. 이전 메뉴로");
            body.push_back("");

            std::vector<std::string> footer = {
                Color("※ 같은 등급 무기 5개(종류 무관)를 소모해 다음 등급 무기 1개를 무작위로 얻습니다.", "90"),
                notice,
                "선택: "
            };

            DrawScreen("등급 합성", body, footer);
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

            if (!ConsumeByRarity(player, from, UPGRADE_REQUIRE_COUNT)) {
                notice = Color("[!] 재료가 부족합니다. ("
                    + std::string(1, rarityChar(from)) + "등급 5개 필요)", "91");
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

        DrawScreen("보급소", body, footer);
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
        case 1: BuyMenu(player, weaponShop, "장비 보급"); break;
        case 2: BuyMenu(player, consumableShop, "소모품 보급"); break;
        case 3: RarityUpgradeMenu(player, itemManager); break;
        case 0: inShop = false; break;
        default: notice = Color("[!] 잘못된 선택입니다.", "91"); break;
        }
    }
}