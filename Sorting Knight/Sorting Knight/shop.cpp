#include "shop.h"
#include "ItemManager.h"
#include "inventory.h"   // Recipe, GetCraftingRecipes() 사용
#include "gameText.h"
#include <iostream>
#include <cmath>
#include <limits>

// 생성자: 넘겨받은 초기 재고 목록을 그대로 저장 (std::move로 불필요한 복사 방지)
Shop::Shop(std::vector<Item> stock) : stock(std::move(stock)) {
}

// ------------------------------------------------------------------
// Buy: 구매 로직. 실패 사유를 구분해서 BuyResult로 반환한다.
// ------------------------------------------------------------------
BuyResult Shop::Buy(Player& player, const std::string& itemName, int count) {
    const Item* item = FindInStock(itemName);
    if (!item) {
        return BuyResult::NOT_FOUND; // 상점에 없는 아이템 (이름 오타 등)
    }
    int totalPrice = item->GetPrice() * count;
    if (player.GetGold() < totalPrice) {
        return BuyResult::NOT_ENOUGH_GOLD; // 골드 부족
    }
    if (player.GetInventory().GetRemainingSpace() < count) {
        return BuyResult::NOT_ENOUGH_SPACE; // 인벤토리 공간 부족 (30개 제한)
    }
    // 여기 도달했다는 건 구매 조건을 전부 통과했다는 뜻 -> 실제 처리
    player.SpendGold(totalPrice);
    player.GetInventory().AddItem(*item, count);
    return BuyResult::SUCCESS;
}

// ------------------------------------------------------------------
// Sell: 판매 로직. 인벤토리에서 아이템을 빼고 그만큼 골드를 지급한다.
// ------------------------------------------------------------------
bool Shop::Sell(Player& player, const std::string& itemName, int count) {
    if (!player.GetInventory().HasItem(itemName, count)) {
        return false; // 보유 수량 부족 (혹은 아예 없음)
    }
    const Item* item = player.GetInventory().FindItem(itemName);
    if (!item) {
        return false; // 이론상 HasItem 통과했으면 여기 안 걸리지만 방어적으로 체크
    }
    int sellPrice = static_cast<int>(std::round(item->GetPrice() * SELL_RATE)) * count;
    player.GetInventory().RemoveItem(itemName, count);
    player.AddGold(sellPrice);
    return true;
}

// 상점 재고 전체를 그대로 반환 (UI.cpp가 화면 그릴 때 사용)
const std::vector<Item>& Shop::GetStock() const {
    return stock;
}

// [디버그/임시용] 콘솔에 상점 재고를 텍스트로 출력
void Shop::ShowStock() const {
    std::cout << "===== 상점 =====" << std::endl;
    for (const auto& item : stock) {
        std::cout << "- " << item.GetName() << " : " << item.GetPrice() << "G"
            << " (판매가 " << static_cast<int>(std::round(item.GetPrice() * SELL_RATE)) << "G)"
            << std::endl;
    }
}

// 재고(stock)에서 이름이 일치하는 아이템을 선형 탐색으로 찾음. 없으면 nullptr.
const Item* Shop::FindInStock(const std::string& itemName) const {
    for (const auto& item : stock) {
        if (item.GetName() == itemName) {
            return &item;
        }
    }
    return nullptr;
}


// ================================================================================
// [ EnterShopMenu 구현부 ]
//
// main.cpp는 게임 시작 시 InitShop(player, &itemManager)를 딱 한 번 호출해두면,
// 이후로는 EnterShopMenu()를 인자 없이 호출하는 것만으로 상점의 모든 동작
// (메뉴 출력 -> 구매 -> 합성 -> 나가기)이 이 파일 안에서 전부 처리된다.
// ================================================================================
namespace {

    // InitShop()에서 등록해둔 Player/ItemManager를 저장하는 내부 static 포인터.
    // main.cpp가 소유권을 갖고 있으므로 여기서는 주소만 빌려서 참조한다(delete 금지).
    Player* g_player = nullptr;
    ItemManager* g_itemManager = nullptr;

    // --------------------------------------------------------------------------
    // 상점이 판매하는 아이템 "이름" 목록.
    // ItemManager::InitializeItems()에 등록된 이름과 철자가 정확히 같아야 함.
    // --------------------------------------------------------------------------
    const std::vector<std::string> weaponItemNames = {
        // C 등급
        "평범한 가위 (C)", "나무 젓가락 (C)", "대나무 빗자루 (C)",
        "분리수거 집게 (C)", "두꺼운 고무장갑 (C)",
        // B 등급
        "커터칼과 라벨 제거기 (B)", "발로 밝는 캔 찌그러뜨리개 (B)", "네오디뮴 자석 석궁 (B)",
        "스티로폼 열선 절단기 (B)", "고압 세척건 (B)", "신문지 해머 (B)",
        "알루미늄 캔 분쇄기 (B)", "산업용 강풍 핑퐁포 (B)",
        // A 등급
        "초음파 유리 분쇄기 (A)", "유압 박스 압축기 (A)", "고온 유기물 분해기 (A)",
        "산업용 회전 분쇄 칼날 (A)", "알루미늄 용해 용광로포 (A)", "레이저 라벨 절단포 (A)",
        "고주파 충격파 캐논 (A)", "폐지 고압 수압포 (A)",
        // S 등급
        "플라즈마 용융 토치 (S)", "클린 월드 엔드 캐논 (S)", "소형 블랙홀 압축기 (S)"
    };

    const std::vector<std::string> consumableItemNames = {
        "박카스 (N)", "응급 구급상자 (N)", "아드레날린 앰플 (N)", "전투 각성제 (N)",
        "특제 단백질 보충제 (N)", "생명력의 영약 (N)", "불법 개조 부품 (N)",
        "장인의 숫돌 (N)", "환경부 장관의 표창장 (N)", "기적의 엘릭서 (N)"
    };

    // itemManager에서 이름 목록에 해당하는 Item들을 뽑아 Shop 재고용 벡터로 만듦.
    std::vector<Item> BuildStock(const ItemManager& itemManager, const std::vector<std::string>& names) {
        std::vector<Item> result;
        result.reserve(names.size());
        for (const auto& name : names) {
            result.push_back(itemManager.GetItem(name));
        }
        return result;
    }

    // 잘못된 입력(문자 등)으로 std::cin이 fail 상태가 되는 것을 막는 헬퍼.
    void ClearCinError() {
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
    }

    // BuyResult에 맞는 안내 메시지 출력
    void PrintBuyResult(BuyResult result, const std::string& itemName) {
        switch (result) {
        case BuyResult::SUCCESS:
            std::cout << "[구매 완료] '" << itemName << "'을(를) 구매했습니다!" << std::endl;
            break;
        case BuyResult::NOT_FOUND:
            std::cout << "[!] 존재하지 않는 아이템입니다." << std::endl;
            break;
        case BuyResult::NOT_ENOUGH_GOLD:
            std::cout << "[!] 골드가 부족합니다." << std::endl;
            break;
        case BuyResult::NOT_ENOUGH_SPACE:
            std::cout << "[!] 인벤토리 공간이 부족합니다. (최대 " << Inventory::MAX_ITEM_COUNT << "개)" << std::endl;
            break;
        }
    }

    // 상점 재고를 번호와 함께 출력
    void ShowShopStock(const Shop& shop) {
        const auto& stock = shop.GetStock();
        if (stock.empty()) {
            std::cout << "(판매 중인 아이템이 없습니다)" << std::endl;
            return;
        }
        for (size_t i = 0; i < stock.size(); ++i) {
            const Item& item = stock[i];
            std::cout << "  " << (i + 1) << ". " << item.GetName()
                << " [" << item.GetRarityString() << "등급] "
                << item.GetPrice() << "G" << std::endl;
        }
    }

    // --------------------------------------------------------------------------
    // BuyMenu: 특정 Shop(무기 상점 / 소비템 상점) 하나를 대상으로 하는
    //          "구매" 화면 루프. 번호 -> 수량 순으로 입력받아 Shop::Buy 호출.
    // --------------------------------------------------------------------------
    void BuyMenu(Player* player, Shop& shop) {
        while (true) {
            std::cout << "\n----------------------------------" << std::endl;
            ShowShopStock(shop);
            std::cout << "----------------------------------" << std::endl;
            std::cout << "보유 골드: " << player->GetGold() << "G   "
                << "인벤토리: " << player->GetInventory().GetTotalItemCount()
                << "/" << Inventory::MAX_ITEM_COUNT << std::endl;
            std::cout << "구매할 아이템 번호 (0: 이전 메뉴로): ";

            int choice;
            if (!(std::cin >> choice)) {
                ClearCinError();
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }
            ClearCinError();

            if (choice == 0) {
                break; // 상위(상점 메인) 메뉴로 복귀
            }

            const auto& stock = shop.GetStock();
            int index = choice - 1;
            if (index < 0 || index >= (int)stock.size()) {
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }

            std::cout << "구매할 수량 (1개 이상): ";
            int count;
            if (!(std::cin >> count)) {
                ClearCinError();
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }
            ClearCinError();
            if (count <= 0) {
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }

            const std::string itemName = stock[index].GetName();
            BuyResult result = shop.Buy(*player, itemName, count);
            PrintBuyResult(result, itemName);
        }
    }

    // --------------------------------------------------------------------------
    // CraftMenu: 등록된 합성 레시피(GetCraftingRecipes())를 번호로 보여주고,
    //            선택하면 player->GetInventory().Craft(...)로 실제 합성 수행.
    // --------------------------------------------------------------------------
    void CraftMenu(Player* player) {
        while (true) {
            const std::vector<Recipe>& recipes = GetCraftingRecipes();

            std::cout << "\n----------------------------------" << std::endl;
            std::cout << "        합성 (제작)" << std::endl;
            std::cout << "----------------------------------" << std::endl;

            if (recipes.empty()) {
                std::cout << "(등록된 합성 레시피가 없습니다)" << std::endl;
            }
            else {
                for (size_t i = 0; i < recipes.size(); ++i) {
                    const Recipe& r = recipes[i];
                    std::cout << "  " << (i + 1) << ". " << r.result.GetName() << "  <- ";
                    for (size_t j = 0; j < r.ingredients.size(); ++j) {
                        std::cout << r.ingredients[j].first << " x" << r.ingredients[j].second;
                        if (j + 1 < r.ingredients.size()) std::cout << ", ";
                    }
                    bool canCraft = player->GetInventory().CanCraft(r.result.GetName());
                    std::cout << (canCraft ? "  [합성 가능]" : "  [재료 부족]") << std::endl;
                }
            }
            std::cout << "  0. 이전 메뉴로" << std::endl;
            std::cout << "합성할 번호 선택: ";

            int choice;
            if (!(std::cin >> choice)) {
                ClearCinError();
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }
            ClearCinError();

            if (choice == 0) {
                break;
            }

            int index = choice - 1;
            if (index < 0 || index >= (int)recipes.size()) {
                std::cout << GameText::MSG_INVALID << std::endl;
                continue;
            }

            const std::string resultName = recipes[index].result.GetName();
            bool success = player->GetInventory().Craft(resultName);
            if (success) {
                std::cout << "[합성 성공] '" << resultName << "'을(를) 획득했습니다!" << std::endl;
            }
            else {
                std::cout << "[!] 재료가 부족하여 합성할 수 없습니다." << std::endl;
            }
        }
    }

} // namespace (익명)


// ================================================================================
// InitShop: 게임 시작 시(Player/ItemManager 생성 직후) 딱 한 번 호출.
//   이후 EnterShopMenu()가 인자 없이도 이 player/itemManager를 사용할 수 있도록
//   내부 static 포인터에 주소를 저장해둔다.
// ================================================================================
void InitShop(Player* player, ItemManager* itemManager) {
    g_player = player;
    g_itemManager = itemManager;
}

// ================================================================================
// EnterShopMenu: main.cpp가 호출하는 유일한 진입점 (인자 없음).
//   InitShop()으로 등록된 player/itemManager를 사용해서
//   "1.기본템 구매 / 2.소비아이템 구매 / 3.합성 / 0.나가기" 메뉴를 반복 처리한다.
// ================================================================================
void EnterShopMenu() {
    if (!g_player || !g_itemManager) {
        // InitShop()을 먼저 호출하지 않고 EnterShopMenu()를 부른 경우에 대한 방어 코드.
        std::cout << "[!] 상점이 초기화되지 않았습니다. InitShop(player, &itemManager)를 먼저 호출하세요." << std::endl;
        return;
    }

    Player* player = g_player;
    ItemManager& itemManager = *g_itemManager;

    // 상점 재고는 방문할 때마다 새로 구성(가격/목록은 고정이므로 매번 같음).
    Shop weaponShop(BuildStock(itemManager, weaponItemNames));
    Shop consumableShop(BuildStock(itemManager, consumableItemNames));

    bool inShop = true;
    while (inShop) {
        std::cout << "\n========== [ 상점 ] ==========" << std::endl;
        std::cout << GameText::MENU_SHOP;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "보유 골드: " << player->GetGold() << "G" << std::endl;
        std::cout << GameText::MSG_PROMPT;

        int choice;
        if (!(std::cin >> choice)) {
            ClearCinError();
            std::cout << GameText::MSG_INVALID << std::endl;
            continue;
        }
        ClearCinError();

        switch (choice) {
        case 1: // 기본템(무기) 구매
            BuyMenu(player, weaponShop);
            break;
        case 2: // 소비아이템 구매
            BuyMenu(player, consumableShop);
            break;
        case 3: // 합성
            CraftMenu(player);
            break;
        case 0: // 나가기 -> 함수 리턴, main의 메인 메뉴로 자동 복귀
            std::cout << "상점을 나갑니다." << std::endl;
            inShop = false;
            break;
        default:
            std::cout << GameText::MSG_INVALID << std::endl;
            break;
        }
    }
}