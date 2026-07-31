#pragma once
#include <string>

// ============================================================
// Job (직업)
// ------------------------------------------------------------
// 플레이어가 선택할 수 있는 직업 종류.
// 캐릭터 생성 시 반드시 하나를 선택해야 하며, 선택한 직업에 따라
// 공격력/체력 보너스와 지급되는 무기 아이템이 달라진다.
// ============================================================
enum class Job
{
    None,           // 무직 (기본값, 실제 플레이 중에는 사용되지 않음)
    Cleaner,        // 청소부        - 무기: 대나무 빗자루
    StreetCleaner,  // 환경미화원    - 무기: 나무 젓가락
    RecycleExpert,  // 분리수거전문가 - 무기: 평범한 가위
    RecycleTech,    // 재활용기사    - 무기: 분리수거 집게
};


// ============================================================
// Player (플레이어)
// ------------------------------------------------------------
// 캐릭터의 이름, 직업, 레벨, 체력, 공격력, 경험치를 관리하는 클래스.
//
// [기본 흐름]
// 1) 이름과 직업을 지정하여 Player 생성 -> 생성자 안에서 직업 스탯이 자동 적용됨
// 2) GainExp()로 경험치 획득 -> 경험치가 100(EXP_PER_LEVEL)을 넘으면 자동 레벨업
// 3) PrintStatus()로 현재 상태를 콘솔에 출력해서 확인
// ============================================================
class Player
{
private:
    // ---------------- 내부 전용 함수 ----------------

    // 레벨을 1 올리고, 레벨에 비례해 체력/공격력을 증가시킨 뒤 체력을 완전 회복한다.
    // GainExp() 내부에서만 호출되는 private 함수.
    void LevelUp();

    // 생성자에서 호출되어, 선택된 직업(newJob)에 맞는 스탯 보너스와
    // 전용 무기 아이템을 캐릭터에게 적용한다.
    void ApplyJob(Job newJob);

    // Job enum 값을 화면에 표시할 한글 문자열로 변환한다. (예: Job::Cleaner -> "청소부")
    static std::string JobToString(Job job);

    // 직업(job)에 대응하는 지급 무기 이름을 반환한다. (예: Job::Cleaner -> "대나무 빗자루")
    static std::string GetItemForJob(Job job);


    // ---------------- 상수 (밸런스 값) ----------------

    static constexpr int MAX_LEVEL = 10;        // 최대 레벨 (이 이상 레벨업 불가)
    static constexpr int EXP_PER_LEVEL = 100;   // 레벨업에 필요한 경험치 (고정값)
    static constexpr int INIT_HP = 200;         // 캐릭터 생성 시 기본 체력
    static constexpr int INIT_ATTACK = 30;      // 캐릭터 생성 시 기본 공격력


    // ---------------- 멤버 변수 ----------------

    std::string name;    // 캐릭터 이름
    int level;            // 현재 레벨
    int hp;                // 현재 체력
    int maxHp;            // 최대 체력 (레벨업/전직으로 증가)
    int attack;           // 공격력 (레벨업/전직으로 증가)
    int exp;               // 현재 경험치 (0 ~ EXP_PER_LEVEL)
    Job job;               // 현재 직업 (enum 값)
    std::string jobName; // 현재 직업 이름 (한글 문자열, 출력용)
    std::string item;    // 직업 전용 지급 무기 이름


public:
    // ---------------- 생성자 ----------------

    // 이름(name)과 초기 직업(initialJob)을 받아 캐릭터를 생성한다.
    // 생성자 내부에서 ApplyJob()을 호출하여 직업 스탯/무기가 즉시 적용된다.
    explicit Player(const std::string& name, Job initialJob);


    // ---------------- 핵심 기능 ----------------

    // 경험치(exp)를 획득한다.
    // 누적 경험치가 EXP_PER_LEVEL(100) 이상이 되면 LevelUp()을 자동으로 호출하며,
    // 한 번에 여러 레벨이 오를 수도 있다. (예: 250 경험치 -> 레벨 2회 상승)
    // 이미 최대 레벨(MAX_LEVEL)이면 아무 효과가 없다.
    void GainExp(int exp);

    // 현재 플레이어 상태(이름/직업/레벨/체력/공격력/경험치/아이템)를
    // 콘솔에 보기 좋게 출력한다. (상태를 바꾸지 않는 const 함수)
    void PrintStatus() const;


    // ---------------- Getter (값 조회) ----------------

    const std::string& GetName() const { return name; }
    int GetLevel() const { return level; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetAttack() const { return attack; }
    int GetExp() const { return exp; }
    Job GetJob() const { return job; }
    const std::string& GetJobName() const { return jobName; } // 직업 이름 (한글, 예: "청소부")
    const std::string& GetItem() const { return item; }       // 직업 전용 지급 무기 이름
    bool IsMaxLevel() const { return level >= MAX_LEVEL; }     // 최대 레벨(10) 도달 여부


    // ---------------- Setter (값 수정) ----------------

    void SetName(const std::string& newName) { name = newName; }
    void SetLevel(int newLevel) { level = newLevel; }
    void SetHp(int newHp);   // 체력은 0 ~ maxHp 범위를 벗어나지 않도록 내부에서 보정(clamp)됨
    void SetMaxHp(int newMaxHp) { maxHp = newMaxHp; }
    void SetAttack(int newAttack) { attack = newAttack; }
    void SetExp(int newExp) { exp = newExp; }
};