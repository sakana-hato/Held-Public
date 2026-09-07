#pragma once
#include "Config.h"

enum class Difficulty
{
	Easy,
	Normal,
	Hard,
};

/// <summary>
/// 難易度に応じたパラメータを返すユーティリティ
/// </summary>
namespace DifficultyParam
{
	//難易度別ボスHP
	inline float BossHP(Difficulty diff)
	{
		switch (diff)
		{
		case Difficulty::Easy:
			return Config::Boss::BOSS_HP_EASE;
		case Difficulty::Normal:
			return Config::Boss::BOSS_HP_NORMAL;
		case Difficulty::Hard:
			return Config::Boss::BOSS_HP_HARD;
		}
		return Config::Boss::BOSS_HP_NORMAL;
	}

	//難易度別ボス攻撃パターン数（簡3 / 普5 / 難7）
	inline int BossAttackCount(Difficulty diff)
	{
		switch (diff)
		{
		case Difficulty::Easy:   return Config::Boss::ATTACK_COUNT_EASE;
		case Difficulty::Normal: return Config::Boss::ATTACK_COUNT_NORMAL;
		case Difficulty::Hard:   return Config::Boss::ATTACK_COUNT_HARD;
		}
		return Config::Boss::ATTACK_COUNT_NORMAL;
	}

	//プレイヤー自動回復(簡)
	inline bool HasHpRegen(Difficulty diff)
	{
		return diff == Difficulty::Easy;
	}

	//敵を常に攻撃する妖精(簡)
	inline bool HasFairy(Difficulty diff)
	{
		return diff == Difficulty::Easy;
	}

	//ボスが魔法を使用するかどうか(普　難)
	inline bool BossUsesMagic(Difficulty diff)
	{
		return diff == Difficulty::Normal || diff == Difficulty::Hard;
	}
}
