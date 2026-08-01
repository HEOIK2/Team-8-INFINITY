#include "shop.h"
#include <iostream>
#include <cmath>

// 생성자: 넘겨받은 초기 재고 목록을 그대로 저장 (std::move로 불필요한 복사 방지)
Shop::Shop(std::vector<Item> stock) : stock(std::move(stock)) {
}

// ------------------------------------------------------------------
// Buy: 구매 로직. 실패 사유를 구분해서 BuyResult로 반환한다.
//
// 검사 순서가 중요함: 없는 아이템 -> 골드 부족 -> 공간 부족 순으로 확인하고,
// 하나라도 걸리면 그 자리에서 즉시 반환(player 상태는 전혀 변경 안 됨).
// 세 조건을 모두 통과했을 때만 실제로 골드를 깎고 아이템을 지급함.
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
// 판매가는 정가의 SELL_RATE(60%)를 반올림한 값 * 판매 개수.
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
