#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include "Item.h"
#include "type.h"

class ItemManager {
private:
    std::unordered_map<std::string, Item> itemDatabase;

public:
    ItemManager() {
        InitializeItems();
    }

    void InitializeItems() {
        // ==========================================
        // 💚 [C 등급] - 기본 도구 (가격: 25원)
        // ==========================================
        RegisterItem(Item(
            "평범한 가위 (C)", "문구용 가위입니다.",
            "싹둑! 테이프와 박스를 절단합니다!",
            ItemRarity::C, { MonsterType::PAPER }, { MonsterType::GLASS, MonsterType::IRON }, 30, 25
        ));

        RegisterItem(Item(
            "나무 젓가락 (C)", "일회용 나무 젓가락입니다.",
            "콕! 쓰레기를 집어 올립니다!",
            ItemRarity::C, { MonsterType::PLASTIC }, { MonsterType::IRON, MonsterType::ALUMINUM }, 28, 25
        ));

        RegisterItem(Item(
            "대나무 빗자루 (C)", "마당을 쓸어내는 빗자루입니다.",
            "쓱싹! 바닥의 쓰레기를 쓸어냅니다!",
            ItemRarity::C, {}, {}, 35, 25
        ));

        RegisterItem(Item(
            "분리수거 집게 (C)", "긴 철제 집게입니다.",
            "착! 찌그러진 캔을 집어 누릅니다!",
            ItemRarity::C, { MonsterType::ALUMINUM }, { MonsterType::GLASS }, 38, 25
        ));

        RegisterItem(Item(
            "두꺼운 고무장갑 (C)", "분홍색 김장용 고무장갑입니다.",
            "찰싹! 유리 파편을 강하게 후려칩니다!",
            ItemRarity::C, { MonsterType::GLASS }, { MonsterType::ALUMINUM }, 32, 25
        ));

        // ==========================================
        // 💙 [B 등급] - 압축 & 중형 장비 (가격: 100원)
        // ==========================================
        RegisterItem(Item(
            "커터칼과 라벨 제거기 (B)", "비닐 라벨 제거용 도구입니다.",
            "슥-삭! 페트병 라벨을 베어냅니다!",
            ItemRarity::B, { MonsterType::PLASTIC }, { MonsterType::IRON, MonsterType::GLASS }, 50, 100
        ));

        RegisterItem(Item(
            "발로 밝는 캔 찌그러뜨리개 (B)", "발판형 캔 압축 장치입니다.",
            "콰직! 캔을 납작하게 만듭니다!",
            ItemRarity::B, { MonsterType::ALUMINUM, MonsterType::PLASTIC }, { MonsterType::IRON }, 55, 100
        ));

        RegisterItem(Item(
            "네오디뮴 자석 석궁 (B)", "자력을 띤 석궁 장치입니다.",
            "철컥! 캔과 알루미늄을 강하게 끌어당깁니다!",
            ItemRarity::B, { MonsterType::ALUMINUM }, { MonsterType::GLASS, MonsterType::PAPER }, 60, 100
        ));

        RegisterItem(Item(
            "스티로폼 열선 절단기 (B)", "열선 절단 도구입니다.",
            "스르륵~ 열선으로 깔끔하게 절단합니다!",
            ItemRarity::B, { MonsterType::PLASTIC }, { MonsterType::GLASS, MonsterType::ALUMINUM }, 58, 100
        ));

        RegisterItem(Item(
            "고압 세척건 (B)", "강력 수압 세척기입니다.",
            "슈아아악! 고압 수압을 뿜어냅니다!",
            ItemRarity::B, { MonsterType::PAPER }, { MonsterType::PLASTIC }, 52, 100
        ));

        RegisterItem(Item(
            "신문지 해머 (B)", "굳힌 신문지 망치입니다.",
            "쿵! 단단한 유리 표면에 금을 냅니다!",
            ItemRarity::B, {}, {}, 54, 100
        ));

        RegisterItem(Item(
            "알루미늄 캔 분쇄기 (B)", "기어식 캔 분쇄기입니다.",
            "드르륵- 콰직! 캔을 구겨버립니다!",
            ItemRarity::B, { MonsterType::ALUMINUM }, { MonsterType::PAPER }, 56, 100
        ));

        RegisterItem(Item(
            "산업용 강풍 핑퐁포 (B)", "대형 송풍 무기입니다.",
            "콰앙! 강풍으로 쓰레기를 밀어냅니다!",
            ItemRarity::B, { MonsterType::PAPER, MonsterType::PLASTIC }, { MonsterType::IRON }, 62, 100
        ));

        // ==========================================
        // 💜 [A 등급] - 전문 파쇄 & 용해 장비 (가격: 250원)
        // ==========================================
        RegisterItem(Item(
            "초음파 유리 분쇄기 (A)", "고주파 유리 분쇄 장비입니다.",
            "콰창! 초음파 진동으로 분쇄합니다!",
            ItemRarity::A, { MonsterType::GLASS, MonsterType::ALUMINUM }, { MonsterType::PAPER }, 80, 250
        ));

        RegisterItem(Item(
            "유압 박스 압축기 (A)", "폐지 압축 기계입니다.",
            "쿵-쾅! 유압 펜치로 박스를 눌러버립니다!",
            ItemRarity::A, { MonsterType::PAPER, MonsterType::PLASTIC }, { MonsterType::IRON }, 85, 250
        ));

        RegisterItem(Item(
            "고온 유기물 분해기 (A)", "플라스틱 용해 장비입니다.",
            "화아아악! 플라스틱을 녹여버립니다!",
            ItemRarity::A, { MonsterType::PLASTIC }, { MonsterType::GLASS }, 88, 250
        ));

        RegisterItem(Item(
            "산업용 회전 분쇄 칼날 (A)", "중공업용 파쇄기입니다.",
            "위잉- 콰쾅! 회전 칼날로 갈아버립니다!",
            ItemRarity::A, { MonsterType::PLASTIC, MonsterType::ALUMINUM }, { MonsterType::IRON }, 82, 250
        ));

        RegisterItem(Item(
            "알루미늄 용해 용광로포 (A)", "캔 용해 정밀 포탑입니다.",
            "팟! 알루미늄을 녹여버립니다!",
            ItemRarity::A, { MonsterType::ALUMINUM }, { MonsterType::GLASS }, 86, 250
        ));

        RegisterItem(Item(
            "레이저 라벨 절단포 (A)", "정밀 레이저 포탑입니다.",
            "찌이잉! 비닐과 종이를 소멸시킵니다!",
            ItemRarity::A, { MonsterType::PAPER, MonsterType::PLASTIC }, { MonsterType::ALUMINUM }, 84, 250
        ));

        RegisterItem(Item(
            "고주파 충격파 캐논 (A)", "충격파 발사 무기입니다.",
            "콰-앙! 진동 충격파를 발사합니다!",
            ItemRarity::A, {}, {}, 87, 250
        ));

        RegisterItem(Item(
            "폐지 고압 수압포 (A)", "초고압 수압포입니다.",
            "슈화아악! 물줄기로 구조를 해체합니다!",
            ItemRarity::A, { MonsterType::PAPER }, { MonsterType::GLASS }, 83, 250
        ));

        // ==========================================
        // 💛 [S 등급] - 종결자 무기 (가격: 500원)
        // ==========================================
        RegisterItem(Item(
            "플라즈마 용융 토치 (S)", "철제 용해용 초고온 토치입니다.",
            "화아아악! IRON 속성의 철판을 녹여버립니다!",
            ItemRarity::S, { MonsterType::IRON }, {}, 150, 500
        ));

        RegisterItem(Item(
            "클린 월드 엔드 캐논 (S)", "최종 정화 병기입니다.",
            "콰콰콰쾅! 궁극의 정화 레이저를 발사합니다!",
            ItemRarity::S, {}, {}, 130, 500
        ));

        RegisterItem(Item(
            "소형 블랙홀 압축기 (S)", "초중력 발생 장치입니다.",
            "쿠구구구... 초중력으로 쓰레기를 한 점으로 압축합니다!",
            ItemRarity::S, { MonsterType::PAPER, MonsterType::PLASTIC, MonsterType::GLASS, MonsterType::ALUMINUM }, {}, 160, 500
        ));

        // ==========================================
        // 🤍 [N 등급] - 소비 아이템 (총 10개)
        // ==========================================

        // 1. 전투 중 체력 회복
        RegisterItem(Item(
            "박카스 (N)", "피로를 회복해주는 드링크입니다.",
            "벌컥벌컥! 체력이 50 회복되었습니다!",
            ItemRarity::N, {}, {}, 0, 15,
            ItemCategory::CONSUMABLE, 50, 0, 0, 0, false
        ));
        RegisterItem(Item(
            "응급 구급상자 (N)", "위급한 순간 체력을 대량으로 회복합니다.",
            "치이익! 응급 처치 완료! (체력 150 회복)",
            ItemRarity::N, {}, {}, 0, 45,
            ItemCategory::CONSUMABLE, 150, 0, 0, 0, false
        ));

        // 2. 전투 중 공격력 증가
        RegisterItem(Item(
            "아드레날린 앰플 (N)", "이번 전투 동안 근력을 일시적으로 강화합니다.",
            "힘이 솟구칩니다! (전투 중 공격력 20 증가)",
            ItemRarity::N, {}, {}, 0, 30,
            ItemCategory::CONSUMABLE, 0, 20, 0, 0, false
        ));
        RegisterItem(Item(
            "전투 각성제 (N)", "부작용을 감수하고 전투력을 극대화합니다.",
            "크아아악! 눈앞이 붉게 물듭니다! (전투 중 공격력 50 증가)",
            ItemRarity::N, {}, {}, 0, 100,
            ItemCategory::CONSUMABLE, 0, 50, 0, 0, false
        ));

        // 3. 베이스 최대 체력 증가 (영구)
        RegisterItem(Item(
            "특제 단백질 보충제 (N)", "근육을 펌핑시켜 최대 체력을 영구적으로 늘려줍니다.",
            "우와아아! 근육 펌핑! (최대체력 30 영구 증가)",
            ItemRarity::N, {}, {}, 0, 150,
            ItemCategory::CONSUMABLE, 0, 0, 30, 0, true
        ));
        RegisterItem(Item(
            "생명력의 영약 (N)", "생명력을 영구적으로 대폭 상승시킵니다.",
            "몸이 가벼워집니다! (최대체력 100 영구 증가)",
            ItemRarity::N, {}, {}, 0, 350,
            ItemCategory::CONSUMABLE, 0, 0, 100, 0, true
        ));

        // 4. 베이스 공격력 증가 (영구)
        RegisterItem(Item(
            "불법 개조 부품 (N)", "도구를 영구적으로 날카롭게 개조합니다.",
            "끼릭끼릭... 무기가 강화되었습니다! (기본 공격력 15 영구 증가)",
            ItemRarity::N, {}, {}, 0, 100,
            ItemCategory::CONSUMABLE, 0, 15, 0, 0, true
        ));
        RegisterItem(Item(
            "장인의 숫돌 (N)", "장인의 기술로 무기를 극한까지 벼려냅니다.",
            "챙그랑! 무기가 눈부시게 빛납니다! (기본 공격력 35 영구 증가)",
            ItemRarity::N, {}, {}, 0, 300,
            ItemCategory::CONSUMABLE, 0, 35, 0, 0, true
        ));

        // 5. 레벨 업 & 모든 스탯 증가 사기 아이템
        RegisterItem(Item(
            "환경부 장관의 표창장 (N)", "명예로운 훈장입니다. 즉시 레벨이 오릅니다.",
            "찬란한 빛과 함께 한 단계 성장했습니다! (즉시 1레벨 상승)",
            ItemRarity::N, {}, {}, 0, 300,
            ItemCategory::CONSUMABLE, 0, 0, 0, 1, false // gainLevel 1
        ));
        RegisterItem(Item(
            "기적의 엘릭서 (N)", "전설 속의 영약으로 모든 능력이 비약적으로 상승합니다.",
            "몸이 완벽하게 재구성됩니다! (최대체력 200, 기본 공격력 50 영구 증가)",
            ItemRarity::N, {}, {}, 0, 1000,
            ItemCategory::CONSUMABLE, 0, 50, 200, 0, true // buffATK 50, buffMaxHP 200, isPermanent true
        ));
    }

    void RegisterItem(const Item& item) {
        itemDatabase.insert({ item.GetName(), item });
    }

    Item GetItem(const std::string& name) const {
        auto it = itemDatabase.find(name);
        if (it != itemDatabase.end()) {
            return it->second;
        }
        return Item("더미 아이템", "", "", ItemRarity::C, {}, {}, 0, 0);
    }

    Item GetRandomItemByRarity(ItemRarity rarity) const {
        std::vector<Item> candidates;

        for (const auto& pair : itemDatabase) {
            if (pair.second.GetRarity() == rarity && pair.second.GetCategory() == ItemCategory::WEAPON) {
                candidates.push_back(pair.second);
            }
        }

        if (!candidates.empty()) {
            int randomIndex = rand() % candidates.size();
            return candidates[randomIndex];
        }

        return GetItem("평범한 가위 (C)");
    }
};