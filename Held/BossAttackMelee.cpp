#include "DxLib.h"
#include "Boss.h"
#include "Config.h"
#include "ResourceManager.h"
#include "BossAttackMelee.h"

void BossAttackMelee::OnStart(Boss& boss)
{
	timer_ = 0.0f;

	//プレイヤーの方を向く
	boss.Comp().facingYawDeg = boss.YawToPlayerDeg();
	boss.SetHitPlayer(false);

	int atk = -1;
	//攻撃アニメ
	if (boss.IsSwordDrawn())
	{
		atk = ResourceManager::Instance().Model("boss_attack_sword");   //大剣用
	}
	else
	{
		atk = ResourceManager::Instance().Model("boss_attack1");         //素手用
	}
	if (atk >= 0)
	{
		boss.PlayAnim(atk, 0, false);
	}

	//攻撃判定を出す手のボーン
	handFrame_ = MV1SearchFrame(boss.ModelHandle(), "mixamorig:LeftHand");
	boss.SetAttackFrame(handFrame_);
	boss.SetAttackActive(false);
}

bool BossAttackMelee::Update(Boss& boss, float dt)
{
	timer_ += dt;

	//判定のON/OFF
	const bool inHitWindow =
		(timer_ >= Config::Boss::Melee::HIT_START &&
			timer_ <= Config::Boss::Melee::HIT_END);
	boss.SetAttackActive(inHitWindow);

	//攻撃終了
	if (timer_ >= Config::Boss::Melee::DURATION)
	{
		return false;   //終了
	}
	return true;        //継続
}

void BossAttackMelee::OnEnd(Boss& boss)
{
	boss.SetAttackActive(false);
}

bool BossAttackMelee::IsUsable(const Boss& boss) const
{
	//近接なので、攻撃距離内でだけ使える
	return boss.DistanceToPlayer() <= Config::Boss::ATTACK_RANGE;
}

bool BossAttackMelee::IsJustDodgeWindow(const Boss& boss) const
{
	return false;
}