#include "DxLib.h"
#include "BossAttackShockwave.h"
#include "Config.h"
#include "Boss.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"
#include "CameraSystem.h"


void BossAttackShockwave::OnStart(Boss& boss)
{
	step = Step::Charge;
	timer = 0.0f;
	hit = false;
	count = 0;
	boss.SetHitPlayer(false);

	//振りかぶりアニメ
	const int anim = ResourceManager::Instance().Model("boss_shockwave");
	if (anim >= 0)
	{
		boss.PlayAnim(anim, 0, false);
	}

	//警告円を足元に出す
	boss.Warning().Init(boss.Comp().pos, Config::Boss::Shockwave::RADIUS, Config::Boss::Shockwave::CHARGE_TIME);
}

bool BossAttackShockwave::Update(Boss& boss, float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	switch (step)
	{
	case Step::Charge:
	{
		boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

		//この回の溜め時間（1回目はCHARGE_TIME、2回目以降はREPEAT_TIME）
		const float chargeTime = (count == 0)
			? Config::Boss::Shockwave::CHARGE_TIME
			: Config::Boss::Shockwave::REPEAT_TIME;

		if (timer >= chargeTime)
		{
			//衝撃波発動
			{
				Player& pl = boss.GetPlayer();
				VECTOR d = VSub(pl.GetPosition(), comp.pos);
				d.y = 0.0f;
				const float dist = VSize(d);
				const float floorY = boss.FloorYAt(comp.pos);
				const bool airborne = (pl.GetPosition().y > floorY + 50.0f);

				if (dist <= Config::Boss::Shockwave::RADIUS && !airborne)
				{
					pl.TakeDamage(Config::Boss::Shockwave::POWER, comp.pos);
				}
			}

			boss.Camera().AddShake(18.0f);
			boss.Warning().Stop();

			count++;
			timer = 0.0f;

			if (count >= Config::Boss::Shockwave::COUNT)
			{
				//3回やったら終了へ
				step = Step::Done;
			}
			else
			{
				//次の衝撃波の溜めへ。警告円をまた出す
				step = Step::Burst;   //短い間を置く
			}
		}
		break;
	}

	case Step::Burst:
	{
		//衝撃波の間の短いインターバル（連続感を出す）
		if (timer >= 0.2f)
		{
			//次の溜めへ。警告円を出す
			step = Step::Charge;
			timer = 0.0f;
			const int anim = ResourceManager::Instance().Model("boss_shockwave");

			if (anim >= 0)
			{
				boss.PlayAnimImmediate(anim, 0, false);
			}
			boss.Warning().Init(comp.pos, Config::Boss::Shockwave::RADIUS, Config::Boss::Shockwave::REPEAT_TIME);
		}
		break;
	}

	case Step::Done:
		return false;
	}

	return true;
}

void BossAttackShockwave::OnEnd(Boss& boss)
{
	boss.SetAttackActive(false);
	boss.Warning().Stop();
}

bool BossAttackShockwave::IsUsable(const Boss& boss) const
{
	//近 中距離で使える（近づきすぎたプレイヤーを狙う）
	return boss.DistanceToPlayer() <= Config::Boss::Shockwave::RADIUS;
}

bool BossAttackShockwave::IsJustDodgeWindow(const Boss& boss) const
{
	if (step != Step::Charge)
	{
		return false;
	}

	const float chargeTime = (count == 0)
		? Config::Boss::Shockwave::CHARGE_TIME
		: Config::Boss::Shockwave::REPEAT_TIME;
	return timer >= chargeTime - 0.1f;
}