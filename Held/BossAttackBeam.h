#pragma once
#include "BossAttack.h"

//直線状の斬撃 フェーズ2の必殺技
class BossAttackBeam final : public BossAttack
{
public:
	void OnStart(Boss& boss) override;
	bool Update(Boss& boss, float dt) override;
	void OnEnd(Boss& boss) override;
	bool IsUsable(const Boss& boss) const override;
	const char* Name() const override { return "Beam"; }

	//斬撃の判定カプセル（仮置き。エフェクトは後で）
	//Boss側から参照できるよう、状態を持つ
	bool  IsFiring() const { return step == Step::Fire; }
	VECTOR BeamStart() const { return beamStart; }
	VECTOR BeamEnd() const { return beamEnd; }

	bool  IsCharging() const { return step == Step::Charge; }
	VECTOR PredictStart() const { return predictStart; }
	VECTOR PredictEnd() const { return predictEnd; }

private:
	enum class Step { Back, Charge, Fire, Done };
	Step  step = Step::Back;
	float timer = 0.0f;
	int beamEffectInstance = -1;   //ビームエフェクトのインスタンス

	VECTOR beamStart = {};
	VECTOR beamEnd = {};
	VECTOR fireDir = {};
	VECTOR predictStart = {};
	VECTOR predictEnd = {};
};
