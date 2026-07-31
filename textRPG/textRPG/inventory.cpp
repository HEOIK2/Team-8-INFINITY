#include "Inventory.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

// ================================================================================
// [합성 레시피 목록 - recipeList]
//
// ★ 새 레시피를 추가하고 싶으면 아래 벡터에 한 줄(Recipe{...})만 추가하면 됨.
//   이 파일의 다른 함수들은 전혀 안 건드려도 자동으로 새 레시피가 반영됨.
//
// 작성 형식:
//   Recipe{
//       결과 아이템(Item 생성자),
//       { {"재료 이름", 필요 개수}, {"재료 이름", 필요 개수}, ... }
//   }
//
// Item 생성자 파라미터 순서 (Item.h 참고):
//   Item(이름, 설명, 공격시 출력텍스트, 등급(ItemRarity), 강한상대(vector), 약한상대(vector), 공격력, 가격)
//
// 예) "고철 조각" 3개 + "캔" 1개로 "녹슨 방패"(B등급, 공격력 15, 가격 80G)를 만드는 레시피:
//   Recipe{
//       Item("녹슨 방패", "고철로 만든 투박한 방패", "방패로 막아낸다!",
//            ItemRarity::B, {}, {}, 15, 80),
//       { {"고철 조각", 3}, {"캔", 1} }
//   }
//
// 주의: 재료 이름("고철 조각" 등)은 실제로 몬스터가 드랍하거나 상점에서 파는
//       아이템의 GetName()과 철자가 정확히 일치해야 함. 오타 나면 그 레시피는
//       평생 합성 불가능 상태가 됨 (CanCraft가 항상 false).
// ================================================================================
namespace {
    std::vector<Recipe> recipeList = {
        // [데모용 예시 레시피] 실제 아이템이 확정되면 이 항목은 지우고 진짜 레시피로 교체하세요.
        Recipe{
            Item("녹슨 방패", "고철로 만든 투박한 방패", "방패로 막아낸다!",
                 ItemRarity::B, {}, {}, 15, 80),
            { {"고철 조각", 3}, {"캔", 1} }
        }

        // 새 레시피는 이 아래에 콤마(,)로 구분해서 계속 추가하면 됨:
        // ,
        // Recipe{
        //     Item("새 아이템 이름", "설명", "공격 텍스트", ItemRarity::A, {}, {}, 20, 100),
        //     { {"재료1", 2}, {"재료2", 1} }
        // }
    };
}

// GetCraftingRecipes(): 위 recipeList를 그대로 외부(UI.cpp 등)에 공개하는 함수.
const std::vector<Recipe>& GetCraftingRecipes() {
    return recipeList;
}

Inventory::Inventory() {
    // 초기 상태는 빈 인벤토리 (items 벡터가 기본 생성자로 이미 비어있음, 별도 처리 불필요)
}

// 현재 인벤토리에 들어있는 "전체 아이템 개수의 합"을 계산.
// 예) {포션 x3, 검 x2} 이면 3+2 = 5 반환. (종류 수가 아니라 개수 총합!)
int Inventory::GetTotalItemCount() const {
    int total = 0;
    for (const auto& slot : items) {
        total += slot.second;
    }
    return total;
}

// 30개 제한(MAX_ITEM_COUNT) 중에서 지금 몇 개 더 넣을 수 있는지 계산.
int Inventory::GetRemainingSpace() const {
    return MAX_ITEM_COUNT - GetTotalItemCount();
}

// ------------------------------------------------------------------
// AddItem: 아이템 추가 로직 (30개 제한 처리 포함)
//
// 동작 순서:
//   1) 남은 공간(remaining)을 계산. 0 이하면 아예 못 넣으므로 바로 0 리턴 + 안내 메시지.
//   2) 넣으려는 개수(count)와 남은 공간(remaining) 중 작은 쪽만큼만 실제로 추가(actualAdd).
//      예) 남은 공간이 5인데 count=10 이면 5개만 넣고 5개는 버려짐(discarded=5).
//   3) 이미 인벤토리에 같은 이름의 아이템이 있으면 그 슬롯의 개수만 증가.
//   4) 없으면 새 슬롯을 만들어서 추가.
//   5) 버려진 개수(discarded)가 있으면 콘솔에 안내 메시지 출력.
//
// ※ 이 함수는 "실패"라는 개념이 없음(항상 뭔가 처리는 됨). 대신 반환값으로
//    "실제로 몇 개 들어갔는지"를 알려줌. 몬스터 드랍처럼 강제로 들어오는
//    상황에서 쓰기 좋음 (호출부에서 실패 처리를 안 해도 되게끔 설계).
//    → 반대로 "공간 없으면 아예 구매를 막고 싶다" 같은 경우(상점 구매)에는
//      이 함수를 부르기 전에 GetRemainingSpace()로 미리 체크해야 함
//      (Shop::Buy()가 그렇게 하고 있음, Shop.cpp 참고).
// ------------------------------------------------------------------
int Inventory::AddItem(const Item& item, int count) {
    int remaining = GetRemainingSpace();
    if (remaining <= 0) {
        // 인벤토리가 이미 꽉 참 -> 하나도 못 넣고 전부 버림
        std::cout << "[!] 인벤토리가 가득 차서 '" << item.GetName()
                   << "' x" << count << "개를 버렸습니다." << std::endl;
        return 0;
    }

    int actualAdd = std::min(count, remaining); // 들어갈 수 있는 만큼만
    int discarded = count - actualAdd;           // 공간 부족으로 못 들어간 개수

    // 이미 같은 아이템이 있으면 개수만 증가시키고 끝
    for (auto& slot : items) {
        if (slot.first.GetName() == item.GetName()) {
            slot.second += actualAdd;
            if (discarded > 0) {
                std::cout << "[!] 인벤토리가 가득 차서 '" << item.GetName()
                           << "' " << discarded << "개는 버려졌습니다." << std::endl;
            }
            return actualAdd;
        }
    }

    // 처음 들어오는 아이템이면 새 슬롯 추가
    items.push_back({ item, actualAdd });
    if (discarded > 0) {
        std::cout << "[!] 인벤토리가 가득 차서 '" << item.GetName()
                   << "' " << discarded << "개는 버려졌습니다." << std::endl;
    }
    return actualAdd;
}

// 이름으로 아이템을 찾아 개수를 줄임. 개수가 0이 되면 슬롯 자체를 삭제(erase).
// 보유 개수보다 많이 지우려 하면 아무것도 지우지 않고 false 반환 (부분 삭제 없음).
bool Inventory::RemoveItem(const std::string& itemName, int count) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->first.GetName() == itemName) {
            if (it->second < count) {
                return false; // 개수 부족
            }
            it->second -= count;
            if (it->second == 0) {
                items.erase(it); // 다 썼으면 슬롯 자체를 제거 (0개짜리 슬롯이 남지 않게)
            }
            return true;
        }
    }
    return false; // 아이템 자체가 인벤토리에 없음
}

// ------------------------------------------------------------------
// UseItem: 소비 아이템 사용 (효과 적용 + 1개 소모)
//   자세한 규칙은 Inventory.h의 주석 참고.
// ------------------------------------------------------------------
UseResult Inventory::UseItem(const std::string& itemName, Player& player) {
    const Item* item = FindItem(itemName);
    if (!item) {
        return UseResult::NOT_FOUND; // 보유하지 않은 아이템
    }
    if (item->GetCategory() != ItemCategory::CONSUMABLE) {
        return UseResult::NOT_CONSUMABLE; // 무기류는 "사용" 불가
    }

    // 효과 적용 (데모용: isPermanent와 무관하게 즉시 적용, 실전투 연동 시 분기 필요)
    if (item->GetHealHP() > 0) {
        player.hp = std::min(player.maxHp, player.hp + item->GetHealHP());
    }
    if (item->GetBuffMaxHP() > 0) {
        player.maxHp += item->GetBuffMaxHP();
        player.hp += item->GetBuffMaxHP(); // 최대체력이 늘어난 만큼 현재체력도 같이 증가
    }
    if (item->GetBuffATK() > 0) {
        player.atk += item->GetBuffATK();
    }
    if (item->GetGainLevel() > 0) {
        player.level += item->GetGainLevel();
    }

    RemoveItem(itemName, 1); // 사용한 만큼 소모
    return UseResult::SUCCESS;
}

// 지정 개수 "이상" 보유 중인지만 확인 (인벤토리 상태는 변경하지 않음)
bool Inventory::HasItem(const std::string& itemName, int count) const {
    for (const auto& slot : items) {
        if (slot.first.GetName() == itemName) {
            return slot.second >= count;
        }
    }
    return false;
}

// 이름으로 아이템의 상세 정보(Item 전체)를 조회. 없으면 nullptr.
// 호출하는 쪽에서 반드시 nullptr 체크할 것.
const Item* Inventory::FindItem(const std::string& itemName) const {
    for (const auto& slot : items) {
        if (slot.first.GetName() == itemName) {
            return &slot.first;
        }
    }
    return nullptr;
}

// 이름으로 보유 개수만 조회. 없으면 0.
int Inventory::GetItemCount(const std::string& itemName) const {
    for (const auto& slot : items) {
        if (slot.first.GetName() == itemName) {
            return slot.second;
        }
    }
    return 0;
}

// UI.cpp가 인벤토리 화면을 그릴 때 쓰는 전체 목록 getter.
const std::vector<std::pair<Item, int>>& Inventory::GetItems() const {
    return items;
}

// ------------------------------------------------------------------
// CanCraft: resultName 아이템을 지금 합성할 수 있는지 판정만 함 (실제 소모 X)
//   1) recipeList에서 result 이름이 resultName과 일치하는 레시피를 찾음
//   2) 레시피가 없으면 false
//   3) 레시피가 요구하는 재료를 HasItem()으로 하나씩 검사, 하나라도 부족하면 false
//   4) 전부 충족하면 true
// ------------------------------------------------------------------
bool Inventory::CanCraft(const std::string& resultName) const {
    const Recipe* recipe = nullptr;
    for (const auto& r : recipeList) {
        if (r.result.GetName() == resultName) {
            recipe = &r;
            break;
        }
    }
    if (!recipe) {
        return false; // 레시피 없음 (이름 오타 가능성 체크할 것)
    }
    for (const auto& ingredient : recipe->ingredients) {
        if (!HasItem(ingredient.first, ingredient.second)) {
            return false; // 재료 부족
        }
    }
    return true;
}

// ------------------------------------------------------------------
// Craft: 실제 합성 수행 (재료 소모 + 결과물 획득)
//   1) CanCraft()로 먼저 가능 여부 확인. 불가능하면 아무 것도 안 하고 false 반환.
//   2) 레시피의 모든 재료를 RemoveItem()으로 소모.
//   3) 완성된 결과 아이템을 AddItem()으로 1개 획득.
//
//   주의: 만약 결과 아이템을 받을 때 인벤토리가 이미 30개로 꽉 차 있다면,
//         재료는 이미 소모된 상태에서 결과물만 버려질 수 있음(AddItem의 정책상).
//         이런 극단적 상황(재료 소모 직후 바로 꽉 차는 경우)은 흔치 않지만,
//         합성 버튼을 누르기 전에 UI에서 여유 공간이 있는지 안내해주면 더 안전함.
// ------------------------------------------------------------------
bool Inventory::Craft(const std::string& resultName) {
    if (!CanCraft(resultName)) {
        return false;
    }
    const Recipe* recipe = nullptr;
    for (const auto& r : recipeList) {
        if (r.result.GetName() == resultName) {
            recipe = &r;
            break;
        }
    }
    // 재료 소모
    for (const auto& ingredient : recipe->ingredients) {
        RemoveItem(ingredient.first, ingredient.second);
    }
    // 결과 아이템 획득
    AddItem(recipe->result, 1);
    return true;
}

// 지금 재료로 합성 가능한 모든 아이템 "이름"을 리스트로 반환.
// (레시피 전체를 순회하면서 CanCraft가 true인 것만 골라냄)
std::vector<std::string> Inventory::GetCraftableList() const {
    std::vector<std::string> result;
    for (const auto& r : recipeList) {
        if (CanCraft(r.result.GetName())) {
            result.push_back(r.result.GetName());
        }
    }
    return result;
}

// [디버그/임시용] 콘솔에 인벤토리 상태를 텍스트로 출력.
// UI.cpp가 완성되면 이 함수 대신 GetItems()를 직접 순회해서 화면을 그리면 됨.
void Inventory::Show() const {
    std::cout << "===== 인벤토리 (" << GetTotalItemCount() << "/" << MAX_ITEM_COUNT << ") =====" << std::endl;
    if (items.empty()) {
        std::cout << "(비어 있음)" << std::endl;
        return;
    }
    for (const auto& slot : items) {
        std::cout << "- " << slot.first.GetName() << " x" << slot.second
                   << " (" << slot.first.GetPrice() << "G, "
                   << slot.first.GetRarityString() << "등급)" << std::endl;
    }
}

// [디버그/임시용] 콘솔에 전체 레시피와 "합성 가능" 여부를 함께 출력.
void Inventory::ShowRecipes() const {
    std::cout << "===== 합성 레시피 =====" << std::endl;
    if (recipeList.empty()) {
        std::cout << "(등록된 레시피 없음)" << std::endl;
        return;
    }
    for (const auto& r : recipeList) {
        std::cout << "- " << r.result.GetName() << " <- ";
        for (size_t i = 0; i < r.ingredients.size(); ++i) {
            std::cout << r.ingredients[i].first << " x" << r.ingredients[i].second;
            if (i + 1 < r.ingredients.size()) std::cout << ", ";
        }
        std::cout << (CanCraft(r.result.GetName()) ? "  [합성 가능]" : "  [재료 부족]") << std::endl;
    }
}
