#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>

#include "main.h"
#include "type.h"
#include "gameText.h"
#include "Item.h"
#include "ItemManager.h"
#include "inventory.h"
#include "player.h"
#include "monster.h"
#include "battle.h"
#include "shop.h"
#include "ui.h"
#include "gameLog.h"
#include "asciiArt.h"

// ============================================================================
//  Recycling Knight - 마지막 공무원의 기록
// ----------------------------------------------------------------------------
//  게임 전체 흐름 (기획서 기준):
//    타이틀 로고 → 타이틀 메뉴 → 캐릭터 생성 → 오프닝
//    → 마을(상태보기/던전/상점/인벤토리)
//    → 던전 3종 (쓰레기장 / 분리수거장·중간보스 / 폐기처리장·최종보스)
//    → 보스 처치 시 엔딩, 패배 시 게임 오버
// ============================================================================

// [헬퍼 함수] 입력 오류(문자 입력 등) 방지
void ClearInputBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
}

// 상점 NPC의 화면 표시 정보 (아트 + 이름 + 한 마디)
static ActorDisplay MakeNpcDisplay(const std::string& quote) {
    ActorDisplay display;
    display.art = GameArt::ShopNpc();
    display.name = "고물상 박씨";
    display.nameColor = UIColor::Cyan;
    display.statLines = { { "\"" + quote + "\"", UIColor::Gray } };
    return display;
}

// ----------------------------------------------------------------------------
// 타이틀 메뉴: 1(게임 시작) / 2(게임 종료) 반환
// ----------------------------------------------------------------------------
int ShowTitleMenu() {
    ActorDisplay knight;
    knight.art = GameArt::Player();
    knight.name = "재활용 기사단 (정원 1명)";
    knight.nameColor = UIColor::Green;

    std::vector<MenuOption> options = { {"게임 시작"}, {"게임 종료"} };
    int choice = UI_RunMenuSelector("RECYCLING KNIGHT - 마지막 공무원의 기록", { knight }, options,
        "폐지된 법을 집행하러 갈 시간입니다.");

    if (choice == 0) return 1;
    if (choice == 1) return 2;
    return 0; // Esc -> 타이틀 다시 표시
}

// ----------------------------------------------------------------------------
// 오프닝 연출 (기획서 오프닝 텍스트)
// ----------------------------------------------------------------------------
static void ShowOpening() {
    std::vector<UILine> lines = {
        { "2026년 3월 1일,", UIColor::Gray },
        { "「생활폐기물 분리배출 등에 관한 법률」이 폐지되었다.", UIColor::White },
        { "", UIColor::Default },
        { "그날부터 버려진 것들은 자신이 무엇이었는지 잊었다.", UIColor::Gray },
        { "", UIColor::Default },
        { "당신은 폐지된 법을 여전히 집행하는, 마지막 공무원이다.", UIColor::Yellow },
    };
    UI_ShowStory("프롤로그", lines);
}

// ----------------------------------------------------------------------------
// 캐릭터 생성: 이름 입력(박스 안) + 직업 선택 + 임관 안내
// ----------------------------------------------------------------------------
Player* InitializeGame() {
    std::string name;

    UI_ShowInputScreen("캐릭터 생성", {}, "용사(공무원)의 이름을 입력해주세요: ");
    while (true) {
        if (std::cin >> name) {
            ClearInputBuffer();
            if (!name.empty()) break;
        } else {
            ClearInputBuffer();
        }
        UI_ShowInputScreen("캐릭터 생성", {}, "이름은 비워둘 수 없어요. 다시 입력해주세요: ");
    }

    ActorDisplay knight;
    knight.art = GameArt::Player();
    knight.name = name;
    knight.nameColor = UIColor::Green;

    std::vector<MenuOption> jobOptions = {
        { "청소부          - 대나무 빗자루  (ATK+15 / HP+35)" },
        { "환경미화원      - 나무 젓가락    (ATK+10 / HP+40)" },
        { "분리수거전문가  - 평범한 가위    (ATK+35 / HP+15)" },
        { "재활용기사      - 분리수거 집게  (ATK+20 / HP+30)" },
    };
    int jobChoice = UI_RunMenuSelector(name + "님의 직업(전문 분야)을 선택하세요", { knight }, jobOptions,
        "직업에 따라 시작 무기와 보너스 스탯이 달라집니다.");
    if (jobChoice < 0) jobChoice = 0; // 취소 시 기본 직업(청소부)

    Player* player = nullptr;
    switch (jobChoice) {
    case 1:  player = new Player(name, Job::StreetCleaner); break;
    case 2:  player = new Player(name, Job::RecycleExpert); break;
    case 3:  player = new Player(name, Job::RecycleTech);   break;
    default: player = new Player(name, Job::Cleaner);       break;
    }

    UI_ShowMessage("발령", { player->ToActorDisplay() },
        "발령장이 도착했다.\n"
        + UI_Colored(name + " | " + player->GetJobName(), UIColor::Cyan) + "\n"
        "소속: 재활용 기사단 (정원 1명)\n"
        "임무: 미분류 폐기물의 재분류 집행");

    return player;
}

// 직업별 초기 아이템 지급 (전용 무기 + 비상용 박카스 2개)
void GiveInitialItems(Player* player, ItemManager& itemManager) {
    Inventory& inv = player->GetInventory();

    switch (player->GetJob()) {
    case Job::Cleaner:       inv.AddItem(itemManager.GetItem("대나무 빗자루 (C)"), 1); break;
    case Job::StreetCleaner: inv.AddItem(itemManager.GetItem("나무 젓가락 (C)"), 1);   break;
    case Job::RecycleExpert: inv.AddItem(itemManager.GetItem("평범한 가위 (C)"), 1);   break;
    case Job::RecycleTech:   inv.AddItem(itemManager.GetItem("분리수거 집게 (C)"), 1); break;
    default: break;
    }

    inv.AddItem(itemManager.GetItem("박카스 (N)"), 2); // 초반 생존용 지급품
}

// ----------------------------------------------------------------------------
// 상태보기: 스탯 + 민원 처리 대장(처치 집계) (기획서 10번 - map<string,int>)
// ----------------------------------------------------------------------------
static void ShowStatusScreen(Player* player) {
    std::vector<UILine> lines = {
        { "── 인사 기록 카드 ──", UIColor::Cyan },
        { "성명   : " + player->GetName(), UIColor::White },
        { "직급   : " + player->GetJobName(), UIColor::White },
        { "레벨   : Lv." + std::to_string(player->GetLevel()) + (player->IsMaxLevel() ? " (MAX)" : ""), UIColor::White },
        { "체력   : " + UI_MakeGauge(player->GetHp(), player->GetMaxHp(), 14), UIColor::Red },
        { "공격력 : " + std::to_string(player->GetAttack()), UIColor::White },
        { "경력   : EXP " + std::to_string(player->GetExp()) + " / " + std::to_string(player->GetExpPerLevel()), UIColor::Cyan },
        { "수당   : " + std::to_string(player->GetGold()) + "G", UIColor::Yellow },
        { "소지품 : " + std::to_string(player->GetInventory().GetTotalItemCount()) + " / "
                      + std::to_string(Inventory::MAX_ITEM_COUNT), UIColor::White },
        { "", UIColor::Default },
        { "── 민원 처리 대장 ──", UIColor::Cyan },
    };

    if (GameLog_GetKills().empty()) {
        lines.push_back({ "(아직 처리한 민원이 없습니다)", UIColor::Gray });
    }
    else {
        for (const auto& kv : GameLog_GetKills()) {
            lines.push_back({ "· " + kv.first + "  x" + std::to_string(kv.second), UIColor::White });
        }
        lines.push_back({ "누적 처리 실적: " + std::to_string(GameLog_TotalKills()) + "건", UIColor::Green });
    }

    UI_ShowStory("상태 확인", lines);
}

// ----------------------------------------------------------------------------
// 상점 - 구매
// ----------------------------------------------------------------------------
static void RunBuyMenu(Player* player, Shop& shop) {
    while (true) {
        std::vector<MenuOption> options;
        for (const auto& item : shop.GetStock()) {
            options.push_back({ item.GetName() + " - " + std::to_string(item.GetPrice()) + "G",
                                RarityToColor(item.GetRarity()) });
        }
        options.push_back({ "돌아가기" });

        int choice = UI_RunMenuSelector("고물상 - 구매",
            { MakeNpcDisplay("다 누가 버린 거지만, 소독은 했네."), player->ToActorDisplay() }, options,
            "보유 골드: " + UI_Colored(std::to_string(player->GetGold()) + "G", UIColor::Yellow));

        if (choice < 0 || choice == (int)shop.GetStock().size()) return;

        std::string itemName = shop.GetStock()[choice].GetName();
        BuyResult result = shop.Buy(*player, itemName, 1);

        std::string msg;
        switch (result) {
        case BuyResult::SUCCESS:
            msg = UI_Colored(itemName + " 구매 완료!", UIColor::Green) + "\n박씨: \"좋은 선택이야. 원 주인은 몰랐던 가치지.\"";
            break;
        case BuyResult::NOT_ENOUGH_GOLD:
            msg = UI_Colored("골드가 부족합니다.", UIColor::Red) + "\n박씨: \"외상은 사절이야. 나라도 망했는데.\"";
            break;
        case BuyResult::NOT_ENOUGH_SPACE:
            msg = UI_Colored("인벤토리 공간이 부족합니다.", UIColor::Red) + "\n박씨: \"자네도 슬슬 버리는 법을 배워야지.\"";
            break;
        case BuyResult::NOT_FOUND:
            msg = UI_Colored("존재하지 않는 아이템입니다.", UIColor::Red);
            break;
        }
        UI_ShowMessage("고물상 - 구매", { MakeNpcDisplay("또 필요한 건 없고?"), player->ToActorDisplay() }, msg);
    }
}

// ----------------------------------------------------------------------------
// 상점 - 판매 (정가의 60%, 기획서 9번)
// ----------------------------------------------------------------------------
static void RunSellMenu(Player* player, Shop& shop) {
    while (true) {
        const auto& items = player->GetInventory().GetItems();

        // 무기 총 개수 (마지막 무기 판매 방지용)
        int weaponUnits = 0;
        for (const auto& slot : items) {
            if (slot.first.GetCategory() == ItemCategory::WEAPON) weaponUnits += slot.second;
        }

        std::vector<MenuOption> options;
        for (const auto& slot : items) {
            int sellPrice = (int)std::round(slot.first.GetPrice() * SELL_RATE);
            options.push_back({ slot.first.GetName() + " x" + std::to_string(slot.second)
                                + " (판매가 " + std::to_string(sellPrice) + "G)",
                                RarityToColor(slot.first.GetRarity()) });
        }
        options.push_back({ "돌아가기" });

        int choice = UI_RunMenuSelector("고물상 - 판매",
            { MakeNpcDisplay("정가의 60%. 원래 그런 법이야."), player->ToActorDisplay() }, options,
            "보유 골드: " + UI_Colored(std::to_string(player->GetGold()) + "G", UIColor::Yellow));

        if (choice < 0 || choice == (int)items.size()) return;

        const Item& target = items[choice].first;
        std::string itemName = target.GetName();

        if (target.GetCategory() == ItemCategory::WEAPON && weaponUnits <= 1) {
            UI_ShowMessage("고물상 - 판매",
                { MakeNpcDisplay("그건 안 사네."), player->ToActorDisplay() },
                UI_Colored("마지막 무기는 팔 수 없습니다!", UIColor::Red) + "\n박씨: \"맨손으로 쓰레기를 잡을 셈인가?\"");
            continue;
        }

        bool ok = shop.Sell(*player, itemName, 1);
        std::string msg = ok
            ? UI_Colored(itemName + " 판매 완료!", UIColor::Green) + "\n박씨: \"...법은 없어졌지만 시세는 남았지.\""
            : UI_Colored("판매에 실패했습니다.", UIColor::Red);
        UI_ShowMessage("고물상 - 판매", { MakeNpcDisplay("더 팔 건 없고?"), player->ToActorDisplay() }, msg);
    }
}

// ----------------------------------------------------------------------------
// 상점 - 조합 가챠 (기획서 6번: 같은 등급 무기 3개 → 상위 등급 1개, 속성 랜덤)
// ----------------------------------------------------------------------------
static int CountWeaponsOfRarity(Player* player, ItemRarity rarity) {
    int count = 0;
    for (const auto& slot : player->GetInventory().GetItems()) {
        if (slot.first.GetCategory() == ItemCategory::WEAPON && slot.first.GetRarity() == rarity) {
            count += slot.second;
        }
    }
    return count;
}

static bool RemoveWeaponsOfRarity(Player* player, ItemRarity rarity, int count) {
    while (count > 0) {
        std::string target;
        for (const auto& slot : player->GetInventory().GetItems()) {
            if (slot.first.GetCategory() == ItemCategory::WEAPON && slot.first.GetRarity() == rarity) {
                target = slot.first.GetName();
                break;
            }
        }
        if (target.empty()) return false;
        player->GetInventory().RemoveItem(target, 1);
        count--;
    }
    return true;
}

static void RunCraftMenu(Player* player, ItemManager& itemManager) {
    const int NEED = 3; // 조합 필요 개수 (기획서 권장안 n=3)

    while (true) {
        struct CraftRow { ItemRarity from; ItemRarity to; const char* label; };
        std::vector<CraftRow> rows = {
            { ItemRarity::C, ItemRarity::B, "C등급 무기 3개 → B등급 랜덤" },
            { ItemRarity::B, ItemRarity::A, "B등급 무기 3개 → A등급 랜덤" },
            { ItemRarity::A, ItemRarity::S, "A등급 무기 3개 → S등급 랜덤" },
        };
        // S등급은 상위가 없으므로 조합 불가 (기획서 주의사항)

        std::vector<MenuOption> options;
        for (const auto& row : rows) {
            int have = CountWeaponsOfRarity(player, row.from);
            options.push_back({ std::string(row.label) + "  (보유 " + std::to_string(have) + "/" + std::to_string(NEED) + ")",
                                RarityToColor(row.from) });
        }
        options.push_back({ "돌아가기" });

        int choice = UI_RunMenuSelector("고물상 - 조합 가챠",
            { MakeNpcDisplay("재활용의 연금술이지. 결과는 운이고."), player->ToActorDisplay() }, options,
            "같은 등급 무기 " + std::to_string(NEED) + "개를 상위 등급 1개로 조합합니다. 속성은 랜덤!");

        if (choice < 0 || choice == (int)rows.size()) return;

        ItemRarity from = rows[choice].from;
        ItemRarity to = rows[choice].to;

        if (CountWeaponsOfRarity(player, from) < NEED) {
            UI_ShowMessage("고물상 - 조합 가챠",
                { MakeNpcDisplay("재료가 모자라잖나."), player->ToActorDisplay() },
                UI_Colored("재료가 부족합니다!", UIColor::Red) + " 같은 등급 무기가 " + std::to_string(NEED) + "개 필요합니다.");
            continue;
        }

        RemoveWeaponsOfRarity(player, from, NEED);
        Item result = itemManager.GetRandomItemByRarity(to);
        player->GetInventory().AddItem(result, 1);

        UI_ShowMessage("고물상 - 조합 가챠",
            { MakeNpcDisplay("오, 이번 건 좀 나왔군."), player->ToActorDisplay() },
            UI_Colored("조합 성공!", UIColor::Green) + "\n"
            + UI_Colored(result.GetName(), RarityToColor(result.GetRarity())) + " 을(를) 획득했습니다!\n"
            + result.GetDescription());
    }
}

// ----------------------------------------------------------------------------
// 상점 입구 (기획서 9번 - NPC / 구매 / 판매 / 조합)
// ----------------------------------------------------------------------------
void EnterShopMenu(Player* player, ItemManager& itemManager) {
    std::vector<Item> stock = itemManager.GetAllItems();
    std::sort(stock.begin(), stock.end(), [](const Item& a, const Item& b) {
        return a.GetPrice() < b.GetPrice();
    });
    Shop shop(stock);

    while (true) {
        std::vector<MenuOption> options = {
            { "구매" }, { "판매 (정가의 60%)" }, { "조합 가챠" }, { "나가기" }
        };
        int choice = UI_RunMenuSelector("고물상 - 재활용 만물상",
            { MakeNpcDisplay("어서 오게. 버려진 것에도 값은 있지."), player->ToActorDisplay() }, options,
            "보유 골드: " + UI_Colored(std::to_string(player->GetGold()) + "G", UIColor::Yellow));

        if (choice == 0)      RunBuyMenu(player, shop);
        else if (choice == 1) RunSellMenu(player, shop);
        else if (choice == 2) RunCraftMenu(player, itemManager);
        else return;
    }
}

// ----------------------------------------------------------------------------
// 인벤토리: 목록 확인 + 소비 아이템 사용
// ----------------------------------------------------------------------------
void OpenInventory(Player* player) {
    while (true) {
        const auto& items = player->GetInventory().GetItems();

        std::vector<MenuOption> options;
        for (const auto& slot : items) {
            std::string tag = (slot.first.GetCategory() == ItemCategory::WEAPON) ? "[무기]" : "[소모품]";
            options.push_back({ slot.first.GetName() + " x" + std::to_string(slot.second) + "  " + tag,
                                RarityToColor(slot.first.GetRarity()) });
        }
        options.push_back({ "나가기" });

        int choice = UI_RunMenuSelector("인벤토리", { player->ToActorDisplay() }, options,
            "소모품은 선택하면 바로 사용합니다. (" + std::to_string(player->GetInventory().GetTotalItemCount())
            + "/" + std::to_string(Inventory::MAX_ITEM_COUNT) + ")");

        if (choice < 0 || choice == (int)items.size()) return;

        const Item& target = items[choice].first;
        std::string itemName = target.GetName();

        if (target.GetCategory() == ItemCategory::WEAPON) {
            UI_ShowMessage("인벤토리", { player->ToActorDisplay() },
                UI_Colored(itemName, RarityToColor(target.GetRarity())) + "\n"
                + target.GetDescription() + "\n"
                + "공격력 " + std::to_string(target.GetBaseATK()) + " | 무기는 전투에서 사용합니다.");
            continue;
        }

        UseResult result = player->GetInventory().UseItem(itemName, *player);
        std::string msg;
        switch (result) {
        case UseResult::SUCCESS:        msg = UI_Colored(itemName + " 사용 완료!", UIColor::Green); break;
        case UseResult::NOT_CONSUMABLE: msg = "장비 아이템은 사용할 수 없습니다."; break;
        case UseResult::NOT_FOUND:      msg = "그 아이템을 찾을 수 없습니다."; break;
        }
        UI_ShowMessage("인벤토리", { player->ToActorDisplay() }, msg);
    }
}

// ----------------------------------------------------------------------------
// 던전 (기획서 8번 - 쓰레기장 / 분리수거장(중간보스) / 폐기처리장(최종보스))
// ----------------------------------------------------------------------------
enum class DungeonOutcome { BACK_TO_TOWN, GAME_OVER, GAME_CLEAR };

static DungeonOutcome RunBossDungeon(Player* player, ItemManager& itemManager) {
    const std::string place = "폐기처리장";

    if (player->GetLevel() < 10) {
        std::vector<MenuOption> options = { {"그래도 들어간다"}, {"돌아간다"} };
        int c = UI_RunMenuSelector(place + " - 입구", { player->ToActorDisplay() }, options,
            UI_Colored("경고: 권장 레벨 Lv.10 (현재 Lv." + std::to_string(player->GetLevel()) + ")", UIColor::Red));
        if (c != 0) return DungeonOutcome::BACK_TO_TOWN;
    }

    std::vector<UILine> intro = {
        { "폐기처리장 깊은 곳. 산처럼 쌓인 봉투들 사이에서", UIColor::Gray },
        { "그것이 천천히 일어선다.", UIColor::Gray },
        { "", UIColor::Default },
        { "종량제봉투 대마왕: \"법은 폐지됐다.\"", UIColor::Magenta },
        { "종량제봉투 대마왕: \"이제 아무도 너를 임명하지 않아.\"", UIColor::Magenta },
        { "", UIColor::Default },
        { "당신: \"임명장은 여기 있습니다. 제 마음속에.\"", UIColor::Yellow },
    };
    UI_ShowStory(place + " - 최종 결전", intro);

    Monster* boss = CreateBoss(player->GetLevel());

    UI_ShowMessage(place + " - 조우", { boss->ToActorDisplay() },
        UI_Colored("* 최상위 위험 개체가 출현했습니다.", UIColor::Red) + "\n"
        "개체명: " + boss->getName() + " (분류 불가 / 초대형)\n"
        "위험등급: " + UI_Colored("최상", UIColor::Red) + " | 배출 기한: 초과 " + std::to_string(3650) + "일");

    BattleResult result = StartBattle(player, boss, itemManager, place);
    delete boss;

    return (result == BattleResult::PLAYER_LOSE) ? DungeonOutcome::GAME_OVER : DungeonOutcome::GAME_CLEAR;
}

static DungeonOutcome RunNormalDungeon(Player* player, ItemManager& itemManager, int dungeonIdx, bool& midBossDefeated) {
    const std::string place = (dungeonIdx == 0) ? "쓰레기장" : "분리수거장";

    while (true) {
        // ---- 몬스터 생성 ----
        Monster* monster = nullptr;
        std::string rank = (dungeonIdx == 0) ? "하" : "중";
        bool isMidBossBattle = false;

        if (dungeonIdx == 1 && !midBossDefeated) {
            // 분리수거장 첫 진입: 중간보스 등장 (기획서 - 대사와 컨셉)
            std::vector<UILine> intro = {
                { "컨베이어 벨트 끝, 거대한 열기가 느껴진다.", UIColor::Gray },
                { "", UIColor::Default },
                { "소각로의 군주: \"분류 따위 무의미하다.\"", UIColor::Red },
                { "소각로의 군주: \"전부 태우면 그만이다.\"", UIColor::Red },
                { "", UIColor::Default },
                { "당신: \"소각은 최후의 수단입니다. 규정 제4조.\"", UIColor::Yellow },
            };
            UI_ShowStory(place + " - 중간보스", intro);

            monster = CreateMidBoss(player->GetLevel());
            rank = "상";
            isMidBossBattle = true;
        }
        else {
            MonsterType types[] = {
                MonsterType::NONE, MonsterType::PAPER, MonsterType::PLASTIC,
                MonsterType::GLASS, MonsterType::ALUMINUM, MonsterType::IRON
            };
            // 분리수거장은 한 단계 위험한 지역: 몬스터가 플레이어보다 2레벨 높게 계산됨
            int monsterLevel = player->GetLevel() + ((dungeonIdx == 1) ? 2 : 0);
            monster = CreateMonster(types[rand() % 6], monsterLevel);
        }

        // ---- 조우 연출 ----
        UI_ShowMessage(place + " - 조우", { monster->ToActorDisplay() },
            "* 미분류 개체가 출현했습니다.\n"
            "개체명: " + monster->getName() + " (" + MonsterTypeToKorean(monster->getProperty()) + " / 추정)\n"
            "위험등급: " + rank + " | 배출 기한: 초과 " + std::to_string(1 + rand() % 30) + "일");

        // ---- 전투 ----
        BattleResult result = StartBattle(player, monster, itemManager, place);
        delete monster;

        if (result == BattleResult::PLAYER_LOSE) {
            return DungeonOutcome::GAME_OVER;
        }

        if (isMidBossBattle) {
            midBossDefeated = true;
            std::vector<UILine> outro = {
                { "소각로의 불이 꺼진다.", UIColor::Gray },
                { "", UIColor::Default },
                { "소각로의 군주: \"...태우지 않으면, 대체 어쩔 셈이냐.\"", UIColor::Red },
                { "당신: \"다시 쓰는 겁니다. 그게 제 일이고요.\"", UIColor::Yellow },
            };
            UI_ShowStory(place + " - 중간보스 격파", outro);
        }

        // ---- 전투 후 분기 (기획서 전투 시작 조건 ②③: 상점 방문/스킵) ----
        while (true) {
            std::vector<MenuOption> options = { {"계속 수색한다"}, {"상점에 들른다"}, {"마을로 돌아간다"} };
            int c = UI_RunMenuSelector(place + " - 수색", { player->ToActorDisplay() }, options,
                "다음 행동을 선택하세요.");

            if (c == 0) break;                                     // 새 전투 (조건 ② 상점 스킵)
            else if (c == 1) EnterShopMenu(player, itemManager);    // 상점 후 다시 선택 (조건 ③ 상점 후 재전투 가능)
            else return DungeonOutcome::BACK_TO_TOWN;
        }
    }
}

// ----------------------------------------------------------------------------
// 엔딩 / 게임오버 연출
// ----------------------------------------------------------------------------
static void ShowEnding(Player* player) {
    std::vector<UILine> dialogue = {
        { "종량제봉투 대마왕이 마지막으로 물었다.", UIColor::Gray },
        { "", UIColor::Default },
        { "\"법은 이미 없어졌잖아. 왜 아직도 하고 있나.\"", UIColor::Magenta },
        { "", UIColor::Default },
        { "당신은 서류를 덮으며 답했다.", UIColor::Gray },
        { "", UIColor::Default },
        { "\"규정이 없어졌다고 쓰레기가 없어지진 않으니까요.\"", UIColor::Yellow },
    };
    UI_ShowStory("엔딩", dialogue);

    std::vector<UILine> report = {
        { "── 최종 민원 처리 대장 ──", UIColor::Cyan },
        { "담당자: " + player->GetName() + " (" + player->GetJobName() + ", Lv." + std::to_string(player->GetLevel()) + ")", UIColor::White },
        { "", UIColor::Default },
    };
    for (const auto& kv : GameLog_GetKills()) {
        report.push_back({ "· " + kv.first + "  x" + std::to_string(kv.second), UIColor::White });
    }
    report.push_back({ "", UIColor::Default });
    report.push_back({ "누적 처리 실적: " + std::to_string(GameLog_TotalKills()) + "건", UIColor::Green });
    report.push_back({ "", UIColor::Default });
    report.push_back({ "* 미처리 건수: 0. 오늘도 정상 근무.", UIColor::Green });
    UI_ShowStory("엔딩 - 업무 보고", report);

    std::vector<UILine> thanks = {
        { "RECYCLING KNIGHT", UIColor::Cyan },
        { "", UIColor::Default },
        { "- 마지막 공무원의 기록, 끝 -", UIColor::White },
        { "", UIColor::Default },
        { "플레이해 주셔서 감사합니다!", UIColor::Yellow },
        { "", UIColor::Default },
        { "팀 8조 「∞ 도전 (infinity challenge)」", UIColor::Gray },
    };
    UI_ShowStory("THE END", thanks);
}

static void ShowGameOver(Player* player) {
    std::vector<UILine> lines = {
        { "당신은 쓰러졌다.", UIColor::Red },
        { "", UIColor::Default },
        { "처리되지 못한 쓰레기가 거리에 쌓여간다...", UIColor::Gray },
        { "마지막 공무원의 기록은 여기서 끊겼다.", UIColor::Gray },
        { "", UIColor::Default },
        { "누적 처리 실적: " + std::to_string(GameLog_TotalKills()) + "건", UIColor::White },
        { "", UIColor::Default },
        { "[ 게 임 오 버 ]", UIColor::Red },
    };
    UI_ShowStory("순직", lines);
}

// ============================================================================
//  프로그램 진입점
// ============================================================================
int main() {
    srand((unsigned)time(nullptr)); // 랜덤 시드 (몬스터 스탯/드랍이 매판 다르게)

    UI_Init();                              // ANSI 색상/커서 제어 켜기
    UI_ShowSplash(GameText::TITLE_LOGO);    // 타이틀 로고 (아무 키나 누르면 진행)

    bool isProgramRunning = true;

    // [Outer Loop] 타이틀 화면 ↔ 메인 게임
    while (isProgramRunning) {

        int titleChoice = ShowTitleMenu();

        if (titleChoice == 2) {
            std::vector<UILine> bye = {
                { "오늘 업무는 여기까지.", UIColor::White },
                { "", UIColor::Default },
                { "게임을 종료합니다.", UIColor::Gray },
            };
            UI_ShowStory("퇴근", bye);
            isProgramRunning = false;
            break;
        }
        else if (titleChoice != 1) {
            continue; // Esc 등 -> 타이틀 다시 표시
        }

        // ---- 새 게임 세팅 ----
        GameLog_Reset();
        ItemManager itemManager;

        Player* player = InitializeGame();
        if (!player) continue;

        GiveInitialItems(player, itemManager);
        ShowOpening();

        bool midBossDefeated = false;

        // [Inner Loop] 마을(메인 메뉴)
        bool inMainMenu = true;
        while (inMainMenu) {
            std::vector<MenuOption> mainOptions = {
                { "상태보기" },
                { "던전 입장" },
                { "상점 (고물상)" },
                { "인벤토리" },
                { "타이틀로 돌아가기" },
            };
            int mainChoice = UI_RunMenuSelector("마을 - 재활용 기사단 대기소",
                { player->ToActorDisplay() }, mainOptions,
                "오늘도 미분류 폐기물이 거리를 배회하고 있다.");

            if (mainChoice == 0) {
                ShowStatusScreen(player);
            }
            else if (mainChoice == 1) {
                std::vector<MenuOption> dungeonOptions = {
                    { "쓰레기장            (권장 Lv.1~)" },
                    { "분리수거장          (권장 Lv.5~)" },
                    { "폐기처리장          (권장 Lv.10)", UIColor::Red },
                    { "돌아가기" },
                };
                int d = UI_RunMenuSelector("던전 선택", { player->ToActorDisplay() }, dungeonOptions,
                    "어디로 출동할까요?");

                DungeonOutcome outcome = DungeonOutcome::BACK_TO_TOWN;
                if (d == 0 || d == 1) {
                    outcome = RunNormalDungeon(player, itemManager, d, midBossDefeated);
                }
                else if (d == 2) {
                    outcome = RunBossDungeon(player, itemManager);
                }

                if (outcome == DungeonOutcome::GAME_OVER) {
                    ShowGameOver(player);
                    inMainMenu = false;
                }
                else if (outcome == DungeonOutcome::GAME_CLEAR) {
                    ShowEnding(player);
                    inMainMenu = false;
                }
            }
            else if (mainChoice == 2) {
                EnterShopMenu(player, itemManager);
            }
            else if (mainChoice == 3) {
                OpenInventory(player);
            }
            else {
                inMainMenu = false; // 타이틀로 (Esc 포함)
            }
        }

        delete player;
        player = nullptr;
    }
    return 0;
}
