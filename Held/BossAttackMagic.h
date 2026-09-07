#pragma once
#include "BossAttack.h"

class BossAttackMagic final :public BossAttack
{
	void OnStart(Boss& boss) override;
	bool Update(Boss& boss, float dt) override;
	void OnEnd(Boss& boss) override;
	bool IsUsable(const Boss& boss) const override;
	const char* Name() const override { return "Magic"; }

private:
	enum class Step { Back, Charge, Shoot, Done };
	Step  step = Step::Back;
	float timer			= 0.0f;
	int   shotDone		= 0;      //撃った弾の数
	float shotTimer		= 0.0f;  //次の弾までのタイマー
	bool waitingToFire = false;
	int magicCircleInstance = -1;   //手に追従する魔法陣エフェクトのインスタンス
};
