#pragma once
#include "BossAttack.h"

/// <summary>
/// ÉWÉÉÉìÉvçUåÇ
/// </summary>
class BossAttackJump final :public BossAttack
{
public:
	void OnStart(Boss& boss)override;
	bool Update(Boss& boss, float dt)override;
	void OnEnd(Boss& boss)override;
	bool IsUsable(const Boss& boss)const override;
	bool IsJustDodgeWindow(const Boss& boss) const override;
	const char* Name()const override { return "Jump"; }

private:
	enum class Step
	{
		Back,
		Charge,
		Jump,
		Done
	};

	Step step = Step::Back;
	VECTOR startPos = {};
	VECTOR landPos = {};
	float timer = 0.0f;
	float jumpStartY = 0.0f;

};
