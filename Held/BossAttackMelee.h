#pragma once

#include "BossAttack.h"

/// <summary>
/// ボスの近接攻撃
/// </summary>
class BossAttackMelee final : public BossAttack
{
public:
	void OnStart(Boss& boss) override;
	bool Update(Boss& boss, float dt) override;
	void OnEnd(Boss& boss) override;

	bool IsUsable(const Boss& boss) const override;

	bool IsJustDodgeWindow(const Boss& boss) const override;

	const char* Name() const override { return "Melee"; }

private:
	float timer_ = 0.0f;
	int   handFrame_ = -1;   //攻撃判定を出す手のボーン
};
