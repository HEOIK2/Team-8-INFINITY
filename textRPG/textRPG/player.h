#pragma once
#include <string>


enum class Job
{
    None,           // 무직
    Cleaner,        // 청소부
    StreetCleaner,  // 환경미화원
    RecycleExpert,  // 분리수거전문가
    RecycleTech,    // 재활용기사
};


class Player
{
public:
    explicit Player(const std::string& name, Job initialJob);

    
    void GainExp(int exp);


    
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
    

    
    void SetName(const std::string& newName) { name = newName; }
    void SetLevel(int newLevel) { level = newLevel; }
    void SetHp(int newHp);
    void SetMaxHp(int newMaxHp) { maxHp = newMaxHp; }
    void SetAttack(int newAttack) { attack = newAttack; }
    void SetExp(int newExp) { exp = newExp; }

private:
    
    void LevelUp();

    void ApplyJob(Job newJob);

   
    static std::string JobToString(Job job);

    
    static std::string GetItemForJob(Job job);

    static constexpr int MAX_LEVEL = 10;        
    static constexpr int EXP_PER_LEVEL = 100;   
    static constexpr int INIT_HP = 200;         
    static constexpr int INIT_ATTACK = 30;      
     

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