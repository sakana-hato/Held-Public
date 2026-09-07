#pragma once
#include "BossAttack.h"
#include "Precompiled.h"

//突進攻撃
class BossAttackCharge final : public BossAttack
{
public:
	void OnStart(Boss& boss) override;
	bool Update(Boss& boss, float dt) override;
	void OnEnd(Boss& boss) override;
	bool IsUsable(const Boss& boss) const override;
	const char* Name() const override { return "Charge"; }

	bool   IsAiming() const { return step == Step::Aim; }
	VECTOR AimStart() const { return aimStart; }
	VECTOR AimEnd() const { return aimEnd; }
	bool IsDashing() const { return step == Step::Dash; }

	//残像の情報（Bossが描画に使う）
	struct AfterImage
	{
		VECTOR pos;
		float  yawDeg;
		float  life;
		int    animModel = -1;   //アニメのモデルハンドル
		int    animIndex = 0;    //アニメのインデックス
		float  animTime = 0.0f;  //アニメの時間（ポーズ）
	};

	const std::vector<AfterImage>& AfterImages() const { return afterImages_; }

private:
	enum class Step { MoveToOrbit, Orbit, Aim, Dash, Done };
	Step   step = Step::Orbit;
	float  timer = 0.0f;
	int    dashDone = 0;

	VECTOR orbitCenter = {};     //外周の中心
	float  orbitAngle = 0.0f;    //外周の現在角度
	float  orbitProgress = 0.0f; //今の外周で進んだ量

	int chargeEffectInstance = -1;   //突進の走行感エフェクト

	VECTOR dashDir = {};         //突進方向

	std::vector<AfterImage> afterImages_;
	float afterImageTimer = 0.0f;

	VECTOR aimStart = {};
	VECTOR aimEnd = {};
};