#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include "Item.h"
#include "Types.h"

class ItemManager {
private:
    // 게임 내 모든 아이템 도감 ("아이템이름" -> Item)
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
            ItemRarity::C, { MonsterType::PAPER, MonsterType::PLASTIC }, { MonsterType::IRON }, 35, 25
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
            ItemRarity::B, { MonsterType::GLASS }, { MonsterType::IRON }, 54, 100
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
            ItemRarity::A, { MonsterType::GLASS }, { MonsterType::IRON }, 87, 250
        ));

        RegisterItem(Item(
            "폐지 고압 수압포 (A)", "초고압 수압포입니다.",
            "슈화아악! 물줄기로 구조를 해체합니다!",
            ItemRarity::A, { MonsterType::PAPER }, { MonsterType::GLASS }, 83, 250
        ));

        // ==========================================
        // 💛 [S 등급] - 종결자 무기 (가격: 500원)
        // ==========================================

        // 1. IRON 스페셜리스트
        RegisterItem(Item(
            "플라즈마 용융 토치 (S)", "철제 용해용 초고온 토치입니다.",
            "화아아악! IRON 속성의 철판을 녹여버립니다!",
            ItemRarity::S, { MonsterType::IRON }, { MonsterType::NONE }, 150, 500
        ));

        // 2. 모든 타입 제너럴리스트
        RegisterItem(Item(
            "클린 월드 엔드 캐논 (S)", "최종 정화 병기입니다.",
            "콰콰콰쾅! 궁극의 정화 레이저를 발사합니다!",
            ItemRarity::S, { MonsterType::PAPER, MonsterType::PLASTIC, MonsterType::GLASS, MonsterType::ALUMINUM, MonsterType::IRON }, { MonsterType::NONE }, 130, 500
        ));

        // 3. 기타 (IRON 제외 광역 정화)
        RegisterItem(Item(
            "소형 블랙홀 압축기 (S)", "초중력 발생 장치입니다.",
            "쿠구구구... 초중력으로 쓰레기를 한 점으로 압축합니다!",
            ItemRarity::S, { MonsterType::PAPER, MonsterType::PLASTIC, MonsterType::GLASS, MonsterType::ALUMINUM }, { MonsterType::NONE }, 160, 500
        ));
    }

    // 아이템 등록 헬퍼 함수
    void RegisterItem(const Item& item) {
        itemDatabase.insert({ item.GetName(), item });
    }

    // 이름으로 아이템 조회
    Item GetItem(const std::string& name) const {
        auto it = itemDatabase.find(name);
        if (it != itemDatabase.end()) {
            return it->second;
        }
        // 예외용 기본 아이템
        return Item("더미 아이템", "", "", ItemRarity::C, {}, {}, 0, 0);
    }

    // 특정 등급(Rarity)의 아이템 중 랜덤으로 하나를 뽑아서 반환하는 함수
    Item GetRandomItemByRarity(ItemRarity rarity) const {
        std::vector<Item> candidates;

        // 도감 전체를 돌면서 요청한 등급과 일치하는 아이템만 수집
        for (const auto& pair : itemDatabase) {
            if (pair.second.GetRarity() == rarity) {
                candidates.push_back(pair.second);
            }
        }

        // 해당 등급 아이템이 존재하면 무작위 1개 반환
        if (!candidates.empty()) {
            int randomIndex = rand() % candidates.size();
            return candidates[randomIndex];
        }

        // 예외 처리 (해당 등급이 없을 경우 기본 아이템 반환)
        return GetItem("평범한 가위 (C)");
    }
};