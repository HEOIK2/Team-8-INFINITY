#include "player.h"
#include <iostream>
#include <algorithm> // std::clamp 사용


// ============================================================
// 생성자
// ------------------------------------------------------------
// 기본 스탯(INIT_HP, INIT_ATTACK 등)으로 멤버를 초기화한 뒤,
// ApplyJob()을 호출하여 선택된 직업의 보너스 스탯과 무기를 적용한다.
// ============================================================
Player::Player(const std::string& name, Job initialJob)
    : name(name)
    , level(1)
    , hp(INIT_HP)
    , maxHp(INIT_HP)
    , attack(INIT_ATTACK)
    , exp(0)
    , gold(0)
    , job(Job::None)
    , jobName("무직")
{
    ApplyJob(initialJob); // 생성과 동시에 직업 적용
}


// ============================================================
// GainExp - 경험치 획득 & 자동 레벨업 처리
// ------------------------------------------------------------
// 1. 이미 최대 레벨이면 아무 것도 하지 않고 종료
// 2. 파라미터로 받은 exp만큼 누적 경험치(this->exp)를 증가
// 3. 누적 경험치가 EXP_PER_LEVEL(100) 이상인 동안 while 루프로
//    반복 레벨업 처리 (한 번에 여러 레벨 상승 가능)
// 4. 최대 레벨에 도달하면 남은 경험치를 0으로 정리
// ============================================================
void Player::GainExp(int exp)
{
    if (IsMaxLevel())
    {
        return;
    }

    this->exp += exp;

    while (this->exp >= EXP_PER_LEVEL && !IsMaxLevel())
    {
        this->exp -= EXP_PER_LEVEL;
        LevelUp();
    }

    if (IsMaxLevel())
    {
        this->exp = 0;
    }
}


// ============================================================
// ApplyJob - 직업 적용 (생성자 전용 내부 함수)
// ------------------------------------------------------------
// 1. job / jobName 멤버를 갱신
// 2. 직업별로 정해진 공격력/체력 보너스를 더함 (switch-case)
// 3. 체력을 최대치로 회복
// 4. 직업 전용 무기 아이템을 지급하고 콘솔에 안내 메시지 출력
//
// [직업별 스탯 보너스 요약]
//   청소부         : 공격력 +15, 체력 +35
//   환경미화원     : 공격력 +10, 체력 +40
//   분리수거전문가 : 공격력 +35, 체력 +15
//   재활용기사     : 공격력 +20, 체력 +30
// ============================================================
void Player::ApplyJob(Job newJob)
{
    job = newJob;
    jobName = JobToString(job);

    switch (job)
    {
    case Job::Cleaner:
        attack += 15;
        maxHp += 35;
        break;
    case Job::StreetCleaner:
        attack += 10;
        maxHp += 40;
        break;
    case Job::RecycleExpert:
        attack += 35;
        maxHp += 15;
        break;
    case Job::RecycleTech:
        attack += 20;
        maxHp += 30;
        break;
    default:
        break;
    }

    hp = maxHp; // 직업 적용 후 체력을 최대치로 맞춰줌

    std::cout << "[" << name << "] " << jobName << "(으)로 시작합니다! "
        << "(공격력: " << attack << ", 최대 체력: " << maxHp << ")\n";
}


// ============================================================
// SetHp - 체력 설정 (범위 보정 포함)
// ------------------------------------------------------------
// newHp 값을 그대로 대입하지 않고, std::clamp로 0 ~ maxHp 사이로
// 강제 보정한다. (음수 체력이나 최대 체력 초과를 방지)
// ============================================================
void Player::SetHp(int newHp)
{
    hp = std::clamp(newHp, 0, maxHp);
}


// ============================================================
// LevelUp - 레벨업 처리 (내부 전용, GainExp에서만 호출됨)
// ------------------------------------------------------------
// - 레벨을 1 올림
// - 최대체력은 (오른 레벨 x 20)만큼, 공격력은 (오른 레벨 x 5)만큼 증가
// - 체력을 최대치로 완전 회복
// - 레벨업 결과를 콘솔에 출력
// ============================================================
void Player::LevelUp()
{
    level++;

    maxHp += level * 20;
    attack += level * 5;

    hp = maxHp;

    std::cout << "[" << name << "] 레벨 업! Lv." << level
        << " (최대 체력: " << maxHp << ", 공격력: " << attack << ")\n";
}


// ============================================================
// JobToString - Job enum -> 한글 이름 문자열 변환 (내부 전용)
// ============================================================
std::string Player::JobToString(Job job)
{
    switch (job)
    {
    case Job::Cleaner:          return "청소부";
    case Job::StreetCleaner:    return "환경미화원";
    case Job::RecycleExpert:    return "분리수거전문가";
    case Job::RecycleTech:      return "재활용기사";
    default:                    return "무직";
    }
}

// ============================================================
// PrintStatus - 현재 플레이어 상태를 콘솔에 출력
// ------------------------------------------------------------
// 이름 / 직업 / 레벨(MAX 여부 포함) / 체력 / 공격력 / 경험치 / 아이템
// 순서로 정리해서 보여준다. 값을 변경하지 않는 const 함수.
// ============================================================
void Player::PrintStatus() const
{
    std::cout << "\n===== 플레이어 상태 =====\n";
    std::cout << "이름   : " << name << "\n";
    std::cout << "직업   : " << jobName << "\n";
    std::cout << "레벨   : " << level << (IsMaxLevel() ? " (MAX)" : "") << "\n";
    std::cout << "체력   : " << hp << " / " << maxHp << "\n";
    std::cout << "공격력 : " << attack << "\n";
    std::cout << "경험치 : " << exp << " / " << EXP_PER_LEVEL << "\n";
    std::cout << "골드   : " << gold << "\n";
    std::cout << "=========================\n";
}