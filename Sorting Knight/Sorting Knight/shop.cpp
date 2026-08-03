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

    const std::vector<std::string> weaponItemNames = {
        "평범한 가위 (C)", "나무 젓가락 (C)", "대나무 빗자루 (C)",
        "분리수거 집게 (C)", "두꺼운 고무장갑 (C)",
        "커터칼과 라벨 제거기 (B)", "발로 밝는 캔 찌그러뜨리개 (B)", "네오디뮴 자석 석궁 (B)",
        "스티로폼 열선 절단기 (B)", "고압 세척건 (B)", "신문지 해머 (B)",
        "알루미늄 캔 분쇄기 (B)", "산업용 강풍 핑퐁포 (B)",
        "초음파 유리 분쇄기 (A)", "유압 박스 압축기 (A)", "고온 유기물 분해기 (A)",
        "산업용 회전 분쇄 칼날 (A)", "알루미늄 용해 용광로포 (A)", "레이저 라벨 절단포 (A)",
        "고주파 충격파 캐논 (A)", "폐지 고압 수압포 (A)",
        "플라즈마 용융 토치 (S)", "클린 월드 엔드 캐논 (S)", "소형 블랙홀 압축기 (S)"
    };

    const std::vector<std::string> consumableItemNames = {
        "박카스 (N)", "응급 구급상자 (N)", "아드레날린 앰플 (N)", "전투 각성제 (N)",
        "특제 단백질 보충제 (N)", "생명력의 영약 (N)", "불법 개조 부품 (N)",
        "장인의 숫돌 (N)", "환경부 장관의 표창장 (N)", "기적의 엘릭서 (N)"
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

    // BuyResult를 화면에 띄울 안내 문자열로 변환
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

            // 수량 입력 화면
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

    // ── 합성 화면 ──────────────────────────────────────────
    void CraftMenu(Player* player) {
        std::string notice = "";

        while (true) {
            const std::vector<Recipe>& recipes = GetCraftingRecipes();

            std::vector<std::string> body = { "" };
            if (recipes.empty()) {
                body.push_back(Color("  (등록된 합성 규격이 없습니다)", "90"));
            }
            else {
                for (size_t i = 0; i < recipes.size(); ++i) {
                    const Recipe& r = recipes[i];
                    std::string line = "  " + std::to_string(i + 1) + ". " + r.result.GetName() + "   <-  ";
                    for (size_t j = 0; j < r.ingredients.size(); ++j) {
                        line += r.ingredients[j].first + " x" + std::to_string(r.ingredients[j].second);
                        if (j + 1 < r.ingredients.size()) { line += ", "; }
                    }
                    bool canCraft = player->GetInventory().CanCraft(r.result.GetName());
                    line += canCraft ? Color("   [가능]", "92") : Color("   [재료 부족]", "90");
                    body.push_back(line);
                }
            }
            body.push_back("");
            body.push_back("  0. 이전 메뉴로");
            body.push_back("");

            std::vector<std::string> footer = {
                Color("※ 합성 시 하위 등급 재료는 소멸합니다.", "90"),
                notice,
                "규격 번호: "
            };

            DrawScreen("합성", body, footer);
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
            if (index < 0 || index >= (int)recipes.size()) {
                notice = Color("[!] 잘못된 선택입니다.", "91");
                continue;
            }

            const std::string resultName = recipes[index].result.GetName();
            bool success = player->GetInventory().Craft(resultName);
            notice = success
                ? Color("[합성 완료] " + resultName, "92")
                : Color("[!] 재료가 부족합니다.", "91");
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
            "  3. 합성",
            "  0. 나가기",
            ""
        };
        std::vector<std::string> footer = {
            "예산 " + Color(std::to_string(player->GetGold()) + "G", "93"),
            notice,
            "선택: "
        };

        DrawScreen("보급소", body, footer);
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
        case 3: CraftMenu(player); break;
        case 0: inShop = false; break;
        default: notice = Color("[!] 잘못된 선택입니다.", "91"); break;
        }
    }
}