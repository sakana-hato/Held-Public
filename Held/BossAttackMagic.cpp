#include "DxLib.h"
#include "BossAttackMagic.h"
#include "Config.h"
#include "Boss.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"
#include "Projectile.h"
#include "HomingProjectile.h"
#include "ProjectileManager.h"

void BossAttackMagic::OnStart(Boss& boss)
{
	step = Step::Back;
	timer = 0.0f;
	shotDone = 0;
	shotTimer = 0.0f;

	const int back = ResourceManager::Instance().Model("boss_walk_back");
	if (back >= 0)
	{
		boss.PlayAnim(back, 0, true);
	}
}

bool BossAttackMagic::Update(Boss& boss, float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	if (magicCircleInstance >= 0)
	{
		const int leftHand = MV1SearchFrame(boss.ModelHandle(), "mixamorig:RightHand");
		if (leftHand >= 0)
		{
			VECTOR handPos = MV1GetFramePosition(boss.ModelHandle(), leftHand);
			EffectManager::Instance().SetPosition(magicCircleInstance, handPos);
		}

		//プレイヤーの方を向かせる
		VECTOR dir = boss.DirToPlayer();
		const float yaw = std::atan2(dir.x, dir.z);
		EffectManager::Instance().SetRotation(magicCircleInstance, VGet(0.0f, yaw, 0.0f));
	}

	switch (step)
	{ 
	case BossAttackMagic::Step::Back:
	{
		const float dist = boss.DistanceToPlayer();
		const bool needMore = (dist < Config::Boss::Magic::BACK_DISTANCE);
		const bool withinTime = (timer < Config::Boss::Magic::BACK_MAX_TIME);

		if (needMore && withinTime)
		{
			VECTOR away = VScale(boss.DirToPlayer(), -1.0f);
			comp.pos.x += away.x * Config::Boss::Magic::BACK_SPEED * dt;
			comp.pos.z += away.z * Config::Boss::Magic::BACK_SPEED * dt;
			boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);
		}
		else
		{
			step = Step::Charge;
			timer = 0.0f;
			const int cast = ResourceManager::Instance().Model("boss_magic");
			if (cast >= 0)
			{
				boss.PlayAnim(cast, 0, false);
				boss.Anim().SetSpeed(Config::Boss::Magic::ANIM_SPEED);
			}

			const int circle = ResourceManager::Instance().Effect("boss_magic_circle");
			if (circle >= 0)
			{
				VECTOR handPos = comp.pos; handPos.y += 150.0f;
				const int leftHand = MV1SearchFrame(boss.ModelHandle(), "mixamorig:LeftHand");
				if (leftHand >= 0)
				{
					handPos = MV1GetFramePosition(boss.ModelHandle(), leftHand);
				}
				magicCircleInstance = EffectManager::Instance().Play(circle, handPos, Config::Effect::BOSS_MAGIC_CIRCLE_SCALE);
			}
		}
		break;
	}
	case Step::Charge:
	{
		if (timer >= Config::Boss::Magic::CHARGE_TIME)
		{
			step = Step::Shoot;
			shotTimer = 0.0f;
			shotDone = 0;

			//1発目のアニメ再生＋発射待ち
			const int cast = ResourceManager::Instance().Model("boss_magic");
			if (cast >= 0)
			{
				boss.PlayAnimImmediate(cast, 0, false);
				boss.Anim().SetSpeed(Config::Boss::Magic::ANIM_SPEED);
			}
			waitingToFire = true;
		}
		break;
	}
	case Step::Shoot:
	{
		boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);
		shotTimer += dt;

		if (waitingToFire)
		{
			//アニメ再生中、CAST_TO_FIREで発射
			if (shotTimer >= Config::Boss::Magic::CAST_TO_FIRE)
			{
				//発射
				VECTOR pos;
				const int leftHand = MV1SearchFrame(boss.ModelHandle(), "mixamorig:LeftHand");
				if (leftHand >= 0)
				{
					pos = MV1GetFramePosition(boss.ModelHandle(), leftHand);
				}
				else
				{
					pos = comp.pos; pos.y += 150.0f; 
				}

				VECTOR dir = boss.DirToPlayer();
				VECTOR vel = VScale(dir, Config::Boss::Magic::SHOT_SPEED);
				auto bullet = std::make_unique<HomingProjectile>(
					pos, vel, ProjectileOwner::Enemy,
					Config::Boss::Magic::SHOT_RADIUS,
					Config::Boss::Magic::SHOT_POWER,
					Config::Boss::Magic::HOMING_TURN);


				const int inst = EffectManager::Instance().Play(
					ResourceManager::Instance().Effect("magic_orb"),
					pos,
					Config::Effect::MAGIC_ORB_SCALE);
					bullet->SetEffectInstance(inst);

				//発射
					boss.GetProjectiles().Spawn(std::move(bullet));

				shotDone++;
				shotTimer = 0.0f;
				waitingToFire = false;   //発射したので、間隔待ちへ
			}
		}
		else
		{
			//間隔中、SHOT_INTERVALで次のアニメ再生＋発射待ちへ
			if (shotDone < Config::Boss::Magic::SHOT_COUNT &&
				shotTimer >= Config::Boss::Magic::SHOT_INTERVAL)
			{
				const int cast = ResourceManager::Instance().Model("boss_magic");
				if (cast >= 0)
				{
					boss.PlayAnimImmediate(cast, 0, false);
					boss.Anim().SetSpeed(Config::Boss::Magic::ANIM_SPEED);
				}
				shotTimer = 0.0f;
				waitingToFire = true;   //アニメ再生したので、発射待ちへ
			}
		}

		if (shotDone >= Config::Boss::Magic::SHOT_COUNT)
		{
			step = Step::Done;
		}
		break;
	}
	case BossAttackMagic::Step::Done:
		return false;
	}

	return true;
}

void BossAttackMagic::OnEnd(Boss& boss)
{
	if (magicCircleInstance >= 0)
	{
		EffectManager::Instance().Stop(magicCircleInstance);
		magicCircleInstance = -1;
	}
}

bool BossAttackMagic::IsUsable(const Boss& boss) const
{
	//魔法を使う難易度でだけ使える（Normal/Hard）
	//まずは常に使える形でもいいよね
	return true;
}
