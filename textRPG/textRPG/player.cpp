// Player.cpp
#include "Player.h"
#include <iostream>

Player::Player(std::string n, std::string j)
    : name(n), job(j), level(1), hp(200), maxHp(200),
    atk(30), exp(0), gold(0) {}

int Player::Attack() const {
    // TODO: atk 반환
    return atk;
}

void Player::TakeDamage(int dmg) {
    // TODO: hp -= dmg, 0 밑으로 안 내려가게
}

bool Player::IsDead() const {
    // TODO: hp <= 0
    return false;
}

void Player::GainExp(int amount) {
    // TODO: exp += amount
    //       exp >= 100 이면 레벨업 (exp -= 100, level++)
    //       maxHp += level*20, atk += level*5, hp = maxHp
    //       level 10이면 더 안 오름
}

void Player::GainGold(int amount) {
    // TODO
}

bool Player::SpendGold(int amount) {
    // TODO: gold 부족하면 false, 되면 차감 후 true
    return false;
}

void Player::Heal(int amount) {
    // TODO: hp += amount, maxHp 초과 금지
}

void Player::AddAtk(int amount) {
    // TODO: atk += amount
}

void Player::ResetBuff() {
    // TODO: 버프로 올린 atk 원복
    //       (원래 atk를 따로 저장해두는 방법 생각해보기)
}

int Player::GetLevel() const { return level; }
int Player::GetGold()  const { return gold; }
std::string Player::GetJob() const { return job; }

void Player::PrintStatus() const {
    // TODO: 이름/직업/레벨/HP/ATK/EXP/골드 출력
}