#include "battle.h"
#include "Player.h"
#include "monster.h"
#include "Item.h"
#include "ItemManager.h"
#include "inventory.h"
#include "gameLog.h"
#include "ui.h"
#include <algorithm>
#include <cstdlib>

namespace {

    // 상성 판정 결과
    enum class Affinity { STRONG, WEAK, NEUTRAL };

    Affinity JudgeAffinity(const Item& item, MonsterType monsterType) {
        std::vector<MonsterType> strong = item.GetStrongAgainst();
        std::vector<MonsterType> weak = item.GetWeakAgainst();

        if (std::find(strong.begin(), strong.end(), monsterType) != strong.end()) {
            return Affinity::STRONG;
        }
        if (std::find(weak.begin(), weak.end(), monsterType) != weak.end()) {
            return Affinity::WEAK;
        }
        return Affinity::NEUTRAL;
    }

    // 전투 승리 시 30% 확률 아이템 드랍. 무기/소비템 50:50 (기획서 미확정 항목의 권장안 채택)
    // 무기 등급 확률: C 50% / B 30% / A 15% / S 5%
    std::string RollDrop(Player* player, ItemManager& itemManager) {
        if (rand() % 100 >= 30) {
            return "전리품 수거 판정... 실패. (30%)";
        }

        Item drop = [&]() {
            if (rand() % 2 == 0) {
                int roll = rand() % 100;
                ItemRarity rarity =
                    (roll < 50) ? ItemRarity::C :
                    (roll < 80) ? ItemRarity::B :
                    (roll < 95) ? ItemRarity::A : ItemRarity::S;
                return itemManager.GetRandomItemByRarity(rarity);
            }
            return itemManager.GetRandomConsumable();
        }();

        int added = player->GetInventory().AddItem(drop, 1);
        if (added <= 0) {
            return "전리품을 발견했지만 인벤토리가 가득 차서 버렸다... (" + drop.GetName() + ")";
        }
        return "전리품 수거: " + UI_Colored(drop.GetName(), RarityToColor(drop.GetRarity()));
    }

} // namespace

BattleResult StartBattle(Player* player, Monster* monster, ItemManager& itemManager, const std::string& placeName) {

    std::string title = placeName + " - 전투";
    int tempAtkBuff = 0; // "이번 전투 동안만" 유효한 공격력 버프 총량 (종료 시 원상복구)

    while (player->GetHp() > 0 && monster->getHp() > 0) {

        std::vector<ActorDisplay> actors = { player->ToActorDisplay(), monster->ToActorDisplay() };

        // ---- 커맨드 목록 구성: 무기(비소모) 먼저, 소비 아이템(소모) 나중 ----
        std::vector<std::string> pickNames;   // 커맨드 인덱스 -> 아이템 이름
        std::vector<bool> pickIsWeapon;       // 커맨드 인덱스 -> 무기 여부
        std::vector<MenuOption> commands;

        for (const auto& slot : player->GetInventory().GetItems()) {
            const Item& it = slot.first;
            if (it.GetCategory() == ItemCategory::WEAPON) {
                pickNames.push_back(it.GetName());
                pickIsWeapon.push_back(true);
                commands.push_back({ it.GetName() + "  [무기]", RarityToColor(it.GetRarity()) });
            }
        }
        for (const auto& slot : player->GetInventory().GetItems()) {
            const Item& it = slot.first;
            if (it.GetCategory() == ItemCategory::CONSUMABLE) {
                pickNames.push_back(it.GetName());
                pickIsWeapon.push_back(false);
                commands.push_back({ it.GetName() + " x" + std::to_string(slot.second) + "  [소모품]", UIColor::Green });
            }
        }

        std::string log;

        if (commands.empty()) {
            // 이론상 무기는 항상 1개 이상 있지만(마지막 무기 판매 금지), 안전장치로 맨손 공격
            int damage = player->GetAttack();
            monster->setHp(monster->getHp() - damage);
            log += "> 집행 도구가 없다! 맨손으로 후려친다!\n";
            log += "→ " + UI_Colored(std::to_string(damage) + " 데미지", UIColor::Red) + ".\n";
        }
        else {
            int pick = UI_RunMenuSelector(title, actors, commands, "집행할 장비를 선택하세요. (무기는 소모되지 않습니다)");
            if (pick < 0) {
                continue; // 전투 중에는 도망칠 수 없음 - 다시 선택
            }

            const Item* found = player->GetInventory().FindItem(pickNames[pick]);
            if (!found) continue;
            Item selected = *found; // 사용 도중 슬롯이 지워질 수 있으니 복사해두고 진행

            if (pickIsWeapon[pick]) {
                // ---- 무기 공격 (비소모) ----
                log += "> " + UI_Colored(selected.GetName(), RarityToColor(selected.GetRarity())) + "을(를) 집행합니다.\n";
                log += selected.GetAttackText() + "\n";

                // 기획서 8번 권장안: (플레이어 ATK + 아이템 ATK) × 속성 배수
                double damage = player->GetAttack() + selected.GetBaseATK();

                Affinity affinity = JudgeAffinity(selected, monster->getProperty());
                if (affinity == Affinity::STRONG) {
                    damage *= 1.5;
                    log += UI_Colored("→ 재질 일치. 처리 효율 1.5배 적용.", UIColor::Green) + "\n";
                }
                else if (affinity == Affinity::WEAK) {
                    damage /= 1.5;
                    log += UI_Colored("→ 재질 불일치. 처리 효율 저하...", UIColor::Gray) + "\n";
                }
                else {
                    log += "→ 규정 절차대로 처리합니다.\n";
                }

                int finalDamage = (int)damage;
                monster->setHp(monster->getHp() - finalDamage);

                int shownHp = (monster->getHp() > 0) ? monster->getHp() : 0;
                log += "→ " + UI_Colored(std::to_string(finalDamage) + " 데미지", UIColor::Red)
                    + ". 대상 잔여 내구 " + std::to_string(shownHp) + "\n";
            }
            else {
                // ---- 소비 아이템 사용 (1개 소모) ----
                log += "> " + UI_Colored(selected.GetName(), UIColor::Green) + " 사용!\n";
                log += selected.GetAttackText() + "\n";

                if (selected.GetBuffATK() > 0 && !selected.GetIsPermanent()) {
                    tempAtkBuff += selected.GetBuffATK(); // 전투 종료 시 원복
                }

                player->GetInventory().UseItem(selected.GetName(), *player);

                if (selected.GetHealHP() > 0) {
                    log += UI_Colored("→ 내구(체력) +" + std::to_string(selected.GetHealHP()), UIColor::Green) + "\n";
                }
                if (selected.GetBuffATK() > 0) {
                    std::string duration = selected.GetIsPermanent() ? " (영구)" : " (이번 전투 동안)";
                    log += UI_Colored("→ 공격력 +" + std::to_string(selected.GetBuffATK()) + duration, UIColor::Yellow) + "\n";
                }
                if (selected.GetBuffMaxHP() > 0) {
                    log += UI_Colored("→ 최대 체력 +" + std::to_string(selected.GetBuffMaxHP()), UIColor::Green) + "\n";
                }
                if (selected.GetGainLevel() > 0) {
                    log += UI_Colored("→ 레벨 +" + std::to_string(selected.GetGainLevel()), UIColor::Magenta) + "\n";
                }
            }
        }

        // ---- 몬스터 반격 ----
        if (monster->getHp() > 0) {
            monster->Attack(player);
            log += "\n" + UI_Colored("→ " + monster->getName() + "의 반격! "
                + std::to_string(monster->getAtk()) + " 피해.", UIColor::Red);
        }

        std::vector<ActorDisplay> afterTurn = { player->ToActorDisplay(), monster->ToActorDisplay() };
        UI_ShowMessage(title, afterTurn, log);
    }

    // 전투 한정 공격력 버프 원상복구
    if (tempAtkBuff > 0) {
        player->AddAttack(-tempAtkBuff);
    }

    // ---- 패배 ----
    if (player->GetHp() <= 0) {
        return BattleResult::PLAYER_LOSE;
    }

    // ---- 승리 처리: 처치 집계 + 보상 + 드랍 + 레벨업 안내 ----
    GameLog_RecordKill(monster->getName());

    int oldLevel = player->GetLevel();
    int gainedExp = monster->getExpReward();
    player->GainExp(gainedExp);

    int gainedGold = monster->getGoldReward();
    player->AddGold(gainedGold);

    std::string log = UI_Colored("* 처리 완료.", UIColor::Green) + "\n";
    log += monster->getName() + " → " + MonsterTypeToKorean(monster->getProperty()) + "(으)로 재분류되었습니다.\n";
    log += "민원 처리 실적 +1 | 수당 " + UI_Colored(std::to_string(gainedGold) + "G", UIColor::Yellow)
        + " | 경력 " + UI_Colored("EXP +" + std::to_string(gainedExp), UIColor::Cyan) + "\n";

    log += RollDrop(player, itemManager) + "\n";

    if (player->GetLevel() > oldLevel) {
        log += UI_Colored("레벨 업! Lv." + std::to_string(player->GetLevel())
            + " (최대 체력 " + std::to_string(player->GetMaxHp())
            + ", 공격력 " + std::to_string(player->GetAttack()) + ")", UIColor::Green) + "\n";

        if (player->GetLevel() >= 10 && oldLevel < 10) {
            log += UI_Colored("이제 일반 몬스터는 상대도 안 된다!", UIColor::Magenta);
        }
    }

    UI_ShowMessage(title + " - 처리 결과", { player->ToActorDisplay() }, log);
    return BattleResult::PLAYER_WIN;
}
