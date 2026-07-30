#pragma once
#include <string>


enum class Job
{
    None,     // 전직 전
    Warrior,  // 전사
    Mage,     // 마법사
    Archer,   // 궁수
    Priest,   // 성직자
};


class Player
{
public:
    explicit Player(const std::string& name);

    
    void GainExp(int exp);

  
    bool ChangeJob(Job newJob);

    
    void PrintStatus() const;

   
    const std::string& GetName() const { return name; }
    int GetLevel() const { return level; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetAttack() const { return attack; }
    int GetExp() const { return exp; }
    Job GetJob() const { return job; }
    const std::string& GetJobName() const { return jobName; }
    const std::string& GetItem() const { return item; }
    bool IsMaxLevel() const { return level >= MAX_LEVEL; }
    bool HasJob() const { return job != Job::None; }

    
    void SetName(const std::string& newName) { name = newName; }
    void SetLevel(int newLevel) { level = newLevel; }
    void SetHp(int newHp);
    void SetMaxHp(int newMaxHp) { maxHp = newMaxHp; }
    void SetAttack(int newAttack) { attack = newAttack; }
    void SetExp(int newExp) { exp = newExp; }

private:
    
    void LevelUp();

   
    static std::string JobToString(Job job);

    
    static std::string GetItemForJob(Job job);

    static constexpr int MAX_LEVEL = 10;        
    static constexpr int EXP_PER_LEVEL = 100;   
    static constexpr int INIT_HP = 200;         
    static constexpr int INIT_ATTACK = 30;      
    static constexpr int JOB_CHANGE_LEVEL = 5;  

    std::string name;
    int level;
    int hp;
    int maxHp;
    int attack;
    int exp;
    Job job;
    std::string jobName; 
    std::string item;    
};