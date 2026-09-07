#pragma once

/// <summary>
/// BGMの識別子
/// </summary>
enum class BgmId
{
    Normal,     //通常
    Battle,     //通常戦闘
    Boss,       //ボス戦
    Count       //個数
};

/// <summary>
/// SEの判別子
/// </summary>
enum class SeId
{
    Attack,     //攻撃
    Dodge,      //回避
    Count       //個数
};