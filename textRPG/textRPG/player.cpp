#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name)
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

bool Player::ChangeJob(Job newJob)
{
    if (HasJob() || level < JOB_CHANGE_LEVEL || newJob == Job::None)
    {
        std::cout << "[" << name << "] 전직에 실패했습니다. "
            << "(필요 레벨: " << JOB_CHANGE_LEVEL << ", 전직 여부: "
            << (HasJob() ? "이미 전직함" : "전직 가능") << ")\n";
        return false;
    }

    job = newJob;
    jobName = JobToString(job);

    // 직업별 스탯 보너스 적용
    switch (job)
    {
    case Job::Warrior:
        attack += 20;
        maxHp += 50;
        break;
    case Job::Mage:
        attack += 40;
        maxHp -= 20;
        break;
    case Job::Archer:
        attack += 30;
        maxHp += 10;
        break;
    case Job::Priest:
        attack += 10;
        maxHp += 40;
        break;
    default:
        break;
    }

    // 전직 시 체력을 최대치로 회복
    hp = maxHp;

    // 직업 전용 아이템 지급
    item = GetItemForJob(job);

    std::cout << "[" << name << "] " << jobName << "(으)로 전직했습니다! "
        << "(공격력: " << attack << ", 최대 체력: " << maxHp << ")\n";
    std::cout << "[" << name << "] 아이템 [" << item << "] 을(를) 획득했습니다!\n";

    return true;
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
    case Job::Warrior: return "전사";
    case Job::Mage:    return "마법사";
    case Job::Archer:  return "궁수";
    case Job::Priest:  return "성직자";
    default:           return "무직";
    }
}

std::string Player::GetItemForJob(Job job)
{
    switch (job)
    {
    case Job::Warrior: return "강철 대검";
    case Job::Mage:    return "마력의 지팡이";
    case Job::Archer:  return "장궁";
    case Job::Priest:  return "신성한 지팡이";
    default:           return "";
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