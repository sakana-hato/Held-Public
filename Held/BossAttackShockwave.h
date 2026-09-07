#pragma once
#include "BossAttack.h"

/// <summary>
/// ”ÍˆÍÕŒ‚”gUŒ‚
/// </summary>
class BossAttackShockwave final : public BossAttack
{
	void OnStart(Boss& boss) override;
	bool Update(Boss& boss, float dt) override;
	void OnEnd(Boss& boss) override;
	bool IsUsable(const Boss& boss) const override;
	bool IsJustDodgeWindow(const Boss& boss) const override;
	const char* Name() const override { return "Shockwave"; }

	enum class Step { Charge, Burst, Done };
	Step  step = Step::Charge;
	float timer = 0.0f;
	bool  hit = false;
	int   count = 0;
};