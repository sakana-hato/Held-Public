#pragma once

/// <summary>
/// BGMの識別子
/// </summary>
enum class BgmId
{
    Normal,     //通常
    Battle,     //通常戦闘
    Boss,       //ボス戦
    Cave,       //洞窟の環境音
    Title,      //タイトルBGM
    Count       //個数
};

/// <summary>
/// SEの判別子
/// </summary>
enum class SeId
{
    Attack,         //攻撃
    Attack_hit,     //攻撃hit
    Dodge,          //回避
    JustDodge,      //ジャスト回避
    DrawKatana,     //抜刀
    SheatheKatana,  //納刀
    EnemyHit,       //雑魚敵のhit音
    FootstepWalk,   //歩く音
    FootstepRun,    //走る音
    JumpVoice1,     //ジャンプボイス1
    JumpVoice2,     //ジャンプボイス2
    JumpVoice3,     //ジャンプボイス3
    DoorOpen,       //開く音
    DoorClose,      //閉じる音
    CounterRush,    //ラッシュ攻撃
    CounterFinish,  //ラッシュ最後の一撃
    JumpAttackLand, //ジャンプ攻撃の着地音
    TargetLock,     //ターゲットをロックした
    TargetUnlock,   //ターゲットを解除した
    BossRoar,       //ボスの咆哮
    BossJumpLand,   //ボスのジャンプ攻撃の着地
    BossPunch,      //ボスの拳攻撃
    BossSword,      //ボスの大剣
    BossCharge,     //ボスの突進
    TitleVoice,     //タイトル演出のセリフ
    Count           //個数
};