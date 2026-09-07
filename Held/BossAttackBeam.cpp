#include "DxLib.h"
#include "BossAttackBeam.h"
#include "Config.h"
#include "Boss.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"
#include "EffectManager.h"

void BossAttackBeam::OnStart(Boss& boss)
{
	step = Step::Back;
	timer = 0.0f;
	boss.SetHitPlayer(false);

	//離れるアニメ（後退）
	const int back = ResourceManager::Instance().Model("boss_walk_back");
	if (back >= 0)
	{
		boss.PlayAnim(back, 0, true);
	}
}

bool BossAttackBeam::Update(Boss& boss, float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	switch (step)
	{
	case Step::Back:
	{
		const float dist = boss.DistanceToPlayer();
		//行動範囲と時間制限も入れる
		const bool needMore = (dist < Config::Boss::Beam::BACK_DISTANCE);
		const bool withinTime = (timer < 1.2f);

		if (needMore && withinTime)
		{
			VECTOR away = VScale(boss.DirToPlayer(), -1.0f);
			comp.pos.x += away.x * Config::Boss::Beam::BACK_SPEED * dt;
			comp.pos.z += away.z * Config::Boss::Beam::BACK_SPEED * dt;
			boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);
		}
		else
		{
			//ため＋発射のアニメを1本再生
			step = Step::Charge;
			timer = 0.0f;
			boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

			const int anim = ResourceManager::Instance().Model("boss_beam");  
			if (anim >= 0) boss.PlayAnim(anim, 0, false);
		}
		break;
	}

	case Step::Charge:
	{
		//ためる間、プレイヤーの方を向き続ける
		boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

		VECTOR dir = boss.Comp().Forward();
		predictStart = boss.Comp().pos;
		predictStart.y += 100.0f;
		predictEnd = VAdd(predictStart, VScale(dir, Config::Boss::Beam::LENGTH));


		if (timer >= Config::Boss::Beam::CHARGE_TIME)
		{
			step = Step::Fire;
			timer = 0.0f;

			//発射方向を固定してカプセルを作る
			fireDir = boss.Comp().Forward();
			beamStart = boss.Comp().pos;
			beamStart.y += 100.0f;
			beamEnd = VAdd(beamStart, VScale(fireDir, Config::Boss::Beam::LENGTH));

			boss.SetHitPlayer(false);
			

			const float yaw = atan2f(fireDir.x, fireDir.z) + DX_PI_F;
			beamEffectInstance = EffectManager::Instance().Play(
				ResourceManager::Instance().Effect("boss_beam"),
				beamStart,
				Config::Effect::BEAM_SCALE,
				VGet(0.0f, yaw, 0.0f));
		}
		break;
	}

	case Step::Fire:
	{
		//ビームエフェクトを大剣の位置・向きに追従
		if (beamEffectInstance >= 0)
		{
			//大剣の根元（発射地点）
			const Capsule blade = boss.GetSwordBladeCapsule();
			const VECTOR beamOrigin = blade.p0;   //大剣の根元、または p1（先端）

			//大剣の向き（刃の根元→先端の方向）
			VECTOR bladeDir = VSub(blade.p1, blade.p0);
			const float len = VSize(bladeDir);
			if (len > 1e-4f) bladeDir = VScale(bladeDir, 1.0f / len);
			const float yaw = atan2f(bladeDir.x, bladeDir.z);
			const float pitch = -asinf(bladeDir.y);   //上下の傾き

			EffectManager::Instance().SetPosition(beamEffectInstance, beamOrigin);
			EffectManager::Instance().SetRotation(beamEffectInstance, VGet(pitch, yaw, 0.0f));
		}

		if (timer >= Config::Boss::Beam::FIRE_TIME)
		{
			step = Step::Done;
			//ビームエフェクトを止める
			if (beamEffectInstance >= 0)
			{
				EffectManager::Instance().Stop(beamEffectInstance);
				beamEffectInstance = -1;
			}
		}
		break;
	}

	case Step::Done:
		return false;
	}

	return true;
}

void BossAttackBeam::OnEnd(Boss& boss)
{
	boss.SetAttackActive(false);
	if (beamEffectInstance >= 0)
	{
		EffectManager::Instance().Stop(beamEffectInstance);
		beamEffectInstance = -1;
	}
}

bool BossAttackBeam::IsUsable(const Boss& boss) const
{
	//フェーズ2以降
	return boss.IsSwordDrawn();
}