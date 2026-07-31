#pragma once
#ifndef TYPES_H
#define TYPES_H

// 아이템/스킬 등급
enum class ItemRarity {
    N, C, B, A, S
};

// ♻️ 분리수거 5대 속성
enum class MonsterType {
    NONE,       // 무속성 (상성 없음)
    PAPER,      // 종이
    PLASTIC,    // 플라스틱
    GLASS,      // 유리
    ALUMINUM,   // 알루미늄 (캔)
    IRON        // 철 (고철)
};

#endif // TYPES_H
