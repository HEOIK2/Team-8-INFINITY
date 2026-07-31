#pragma once
#include <string>
#include <vector>
#include "Item.h"
#include "Player.h"

// ================================================================================
// [ Shop.h / Shop.cpp ]  -  상점(구매/판매) 시스템
// 담당: (인벤토리 / 상점)
//
// 이 파일이 하는 일
//   - 상점 재고(stock) 목록 관리
//   - 플레이어의 골드/인벤토리와 연동해서 "구매"와 "판매" 처리
//
// 다른 팀원이 알아야 할 것
//   - Player.h  : Shop은 Player&를 받아서 player.gold(int), player.inventory(Inventory)를
//                 직접 읽고 씀. Player.h의 실제 멤버 이름이 이거랑 다르면 Shop.cpp가 컴파일 안 됨
//                 -> 이름이 다르면 Shop.cpp 쪽에서 맞춰줄 테니 알려주면 됨.
//   - Inventory : 구매 시 player.inventory.AddItem(), 판매 시 HasItem()/FindItem()/RemoveItem()을
//                 그대로 가져다 씀. 인벤토리 30개 제한도 구매 시 자동으로 체크됨(BuyResult 참고).
//   - UI.cpp    : GetStock()으로 상점 재고 목록을 받아서 화면에 그리고,
//                 사용자가 아이템을 고르면 Buy()/Sell()을 호출하면 됨.
//   - 합성(Crafting) 기능은 여기 없음. Inventory.h/cpp 쪽 기능임.
// ================================================================================

// 판매가 계산에 쓰이는 비율. 판매가 = 아이템 정가 * SELL_RATE (기획 스펙: 60%)
// 예) 정가 100G 아이템을 팔면 60G를 받음.
const double SELL_RATE = 0.6;

// --------------------------------------------------------------------------------
// [BuyResult] 구매 시도 결과를 나타내는 enum.
//
// Shop::Buy()는 성공 여부를 단순 bool이 아니라 이 enum으로 반환함.
// "왜 실패했는지"까지 알아야 UI에서 정확한 안내 메시지를 보여줄 수 있기 때문.
//
// UI.cpp 등에서 사용하는 방법 (main.cpp의 실제 사용 예):
//     BuyResult result = shop.Buy(player, itemName, count);
//     switch (result) {
//         case BuyResult::SUCCESS:          안내: "구매 성공!"; break;
//         case BuyResult::NOT_ENOUGH_GOLD:  안내: "골드가 부족해요!"; break;
//         case BuyResult::NOT_ENOUGH_SPACE: 안내: "인벤토리 공간이 부족해요!"; break;
//         case BuyResult::NOT_FOUND:        안내: "존재하지 않는 아이템이에요!"; break;
//     }
// --------------------------------------------------------------------------------
enum class BuyResult {
    SUCCESS,           // 구매 성공 (골드 차감 + 아이템 지급 완료)
    NOT_FOUND,         // 상점 재고(stock)에 해당 이름의 아이템이 없음
    NOT_ENOUGH_GOLD,   // player.gold가 총 가격보다 적음
    NOT_ENOUGH_SPACE   // 인벤토리 여유 공간(GetRemainingSpace())보다 구매 개수가 많음
};

// ================================================================================
// [Shop] 상점 구매/판매 로직을 담당하는 클래스
//
// 생성 시 판매할 아이템 목록(stock)을 받아서 들고 있고,
// 이후 Buy()/Sell()로 플레이어와 골드·아이템을 주고받음.
//
// 사용 예시:
//     std::vector<Item> stockList = { Item(...), Item(...) };
//     Shop shop(stockList);
//
//     BuyResult r = shop.Buy(player, "포션", 2);   // 포션 2개 구매 시도
//     bool ok = shop.Sell(player, "낡은 검", 1);    // 낡은 검 1개 판매
// ================================================================================
class Shop {
public:
    explicit Shop(std::vector<Item> stock);

    // ------------------------------------------------------------------
    // Buy(player, itemName, count)
    //   상점 재고에서 itemName 아이템을 count개 구매한다.
    //
    //   처리 순서 (하나라도 실패하면 그 즉시 중단하고 실패 사유 반환,
    //             골드/인벤토리 둘 다 전혀 변경되지 않음 - 부분 처리 없음):
    //     1) 상점 재고에 그 이름의 아이템이 있는지 확인       -> 없으면 NOT_FOUND
    //     2) 총 가격(가격 * count)만큼 골드가 충분한지 확인   -> 부족하면 NOT_ENOUGH_GOLD
    //     3) 인벤토리에 count개 들어갈 공간이 있는지 확인     -> 부족하면 NOT_ENOUGH_SPACE
    //     4) 위 세 조건을 모두 통과하면: 골드 차감 + 인벤토리에 아이템 지급 -> SUCCESS
    //
    //   매개변수:
    //     player   - 구매하는 플레이어 (gold, inventory가 변경됨)
    //     itemName - 상점 재고에 있는 아이템 이름 (GetStock()에서 얻은 이름 그대로 넘기면 됨)
    //     count    - 구매할 개수 (기본값 1)
    //
    //   반환값: BuyResult (위 enum 설명 참고). UI에서 반드시 분기 처리해서
    //           사용자에게 실패 이유를 보여줄 것 (그냥 무시하면 왜 안 샀는지 알 수 없음).
    // ------------------------------------------------------------------
    BuyResult Buy(Player& player, const std::string& itemName, int count = 1);

    // ------------------------------------------------------------------
    // Sell(player, itemName, count)
    //   플레이어 인벤토리에 있는 아이템을 count개 판매한다.
    //   판매가는 해당 아이템 정가의 SELL_RATE(60%)만큼, 반올림해서 지급.
    //
    //   처리 순서:
    //     1) 인벤토리에 count개 이상 보유 중인지 확인 -> 아니면 false
    //     2) 인벤토리에서 count개 제거(RemoveItem)
    //     3) 판매가만큼 player.gold 증가
    //
    //   매개변수:
    //     player   - 판매하는 플레이어 (gold, inventory가 변경됨)
    //     itemName - 플레이어가 실제로 들고 있는 아이템 이름
    //     count    - 판매할 개수 (기본값 1)
    //
    //   반환값: true(판매 성공) / false(보유 개수 부족 - 아무 변화 없음)
    //
    //   주의: 상점 재고(stock)에 없는 아이템(몬스터 드랍템 등)도 판매 가능함.
    //         판매 가능 여부는 "인벤토리에 있는가"로만 판단하고 상점 재고와는 무관.
    // ------------------------------------------------------------------
    bool Sell(Player& player, const std::string& itemName, int count = 1);

    // ------------------------------------------------------------------
    // GetStock()
    //   현재 상점에서 판매 중인 아이템 전체 목록을 반환.
    //   UI.cpp가 상점 화면을 그릴 때 이 목록을 순회하면서 각 아이템의
    //   이름/가격/등급 등을 표시하면 됨 (Item.h의 getter 사용).
    // ------------------------------------------------------------------
    const std::vector<Item>& GetStock() const;

    // ------------------------------------------------------------------
    // ShowStock()
    //   [디버그/임시용] 콘솔에 상점 재고를 텍스트로 출력.
    //   UI.cpp가 완성되면 GetStock()을 직접 써서 화면을 그리면 되므로 이 함수는
    //   대체되거나 삭제되어도 다른 코드에 영향 없음.
    // ------------------------------------------------------------------
    void ShowStock() const;

private:
    std::vector<Item> stock; // 상점이 판매 중인 아이템 목록

    // 재고(stock)에서 이름으로 아이템을 검색하는 내부 helper. 없으면 nullptr.
    // (Buy() 내부에서만 사용되는 private 함수라 외부에서 직접 호출할 필요 없음)
    const Item* FindInStock(const std::string& itemName) const;
};
