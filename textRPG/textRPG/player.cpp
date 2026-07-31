#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, Job initialJob)
    : name(name)
    , level(1)
    , hp(INIT_HP)
    , maxHp(INIT_HP)
    , attack(INIT_ATTACK)
    , exp(0)
    , job(Job::None)
    , jobName("무직")
    , item("")
{

    ApplyJob(initialJob);
}

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

void Player::ApplyJob(Job newJob)
{
    

    job = newJob;
    jobName = JobToString(job);

    // 직업별 스탯 보너스 적용
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

    // 전직 시 체력을 최대치로 회복
    hp = maxHp;

    // 직업 전용 아이템 지급
    item = GetItemForJob(job);

    std::cout << "[" << name << "] " << jobName << "(으)로 시작합니다! "
        << "(공격력: " << attack << ", 최대 체력: " << maxHp << ")\n";


    if (!item.empty())
    {
        std::cout << "[" << name << "] 무기 아이템 [" << item << "] 을(를) 획득했습니다!\n";
    }
}

void Player::SetHp(int newHp)
{
    hp = std::clamp(newHp, 0, maxHp);
}

void Player::LevelUp()
{
    level++;

    maxHp += level * 20;
    attack += level * 5;

    hp = maxHp;

    std::cout << "[" << name << "] 레벨 업! Lv." << level
        << " (최대 체력: " << maxHp << ", 공격력: " << attack << ")\n";
}

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

std::string Player::GetItemForJob(Job job)
{
    switch (job)
    {
    case Job::Cleaner:          return "대나무 빗자루";
    case Job::StreetCleaner:    return "나무 젓가락";
    case Job::RecycleExpert:    return "평범한 가위";
    case Job::RecycleTech:      return "분리수거 집게";
    default:                    return "";
    }
}

void Player::PrintStatus() const
{
    std::cout << "===== 플레이어 상태 =====\n";
    std::cout << "이름   : " << name << "\n";
    std::cout << "직업   : " << jobName << "\n";
    std::cout << "레벨   : " << level << (IsMaxLevel() ? " (MAX)" : "") << "\n";
    std::cout << "체력   : " << hp << " / " << maxHp << "\n";
    std::cout << "공격력 : " << attack << "\n";
    std::cout << "경험치 : " << exp << " / " << EXP_PER_LEVEL << "\n";
    std::cout << "아이템 : " << (item.empty() ? "없음" : item) << "\n";
    std::cout << "=========================\n";

}