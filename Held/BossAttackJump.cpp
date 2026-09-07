#include "DxLib.h"
#include "BossAttackJump.h"
#include "Config.h"
#include "Boss.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"
#include "EffectManager.h"
#include "CameraSystem.h"

void BossAttackJump::OnStart(Boss& boss)
{
	step = Step::Back;
	timer = 0.0f;
	boss.SetHitPlayer(false);

	const int back = ResourceManager::Instance().Model("boss_walk_back");
	if (back >= 0)
	{
		boss.PlayAnim(back, 0, true); 
	}
}

bool BossAttackJump::Update(Boss& boss, float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	switch (step)
	{
	case Step::Back:
	{
		const float dist = boss.DistanceToPlayer();

		//行動範囲の縁を計算
		auto& comp = boss.Comp();
		const VECTOR arenaCenter = VGet(Config::Boss::ARENA_CENTER_X, comp.pos.y, Config::Boss::ARENA_CENTER_Z);
		VECTOR fromCenter = VSub(comp.pos, arenaCenter);
		fromCenter.y = 0.0f;
		const float distFromCenter = VSize(fromCenter);

		//まだ離れ足りない、かつ 時間内、かつ 行動範囲内 ならバック継続
		const bool needMoreDistance = (dist < Config::Boss::Jump::BACK_DISTANCE);
		const bool withinTime = (timer < Config::Boss::Jump::BACK_MAX_TIME);
		const bool withinArena = (distFromCenter < Config::Boss::ARENA_RADIUS);

		if (needMoreDistance && withinTime && withinArena)
		{
			//バック継続
			VECTOR away = VScale(boss.DirToPlayer(), -1.0f);
			comp.pos.x += away.x * Config::Boss::Jump::BACK_SPEED * dt;
			comp.pos.z += away.z * Config::Boss::Jump::BACK_SPEED * dt;

			//行動範囲の縁で止める
			VECTOR fc = VSub(comp.pos, arenaCenter);
			fc.y = 0.0f;
			const float d = VSize(fc);
			if (d > Config::Boss::ARENA_RADIUS)
			{
				const VECTOR clamped = VScale(VNorm(fc), Config::Boss::ARENA_RADIUS);
				comp.pos.x = arenaCenter.x + clamped.x;
				comp.pos.z = arenaCenter.z + clamped.z;
			}

			boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);
		}
		else
		{
			//十分離れた or 時間切れ or 範囲の縁 → 溜めへ
			step = Step::Charge;
			timer = 0.0f;
			
			
			landPos = boss.GetPlayer().GetPosition();
			
			boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

			const float fillTime = Config::Boss::Jump::CHARGE_TIME + Config::Boss::Jump::JUMP_TIME;
			boss.Warning().Init(landPos, Config::Boss::Jump::LAND_RADIUS, fillTime);

			const int anim = ResourceManager::Instance().Model("boss_attack_jump");
			if (anim >= 0) boss.PlayAnim(anim, 0, false);
		}
		break;
	}
	case Step::Charge:
	{
				

		if (timer >= Config::Boss::Jump::CHARGE_TIME)
		{
			step = Step::Jump;
			timer = 0.0f;
			startPos = comp.pos;
			jumpStartY = comp.pos.y;
		}
		break;
	}
	case Step::Jump:
	{
		const float tim = timer / Config::Boss::Jump::JUMP_TIME;

		if (tim < 1.0f)
		{
			comp.pos.x = startPos.x + (landPos.x - startPos.x) * tim;
			comp.pos.z = startPos.z + (landPos.z - startPos.z) * tim;
			comp.pos.y = jumpStartY + std::sin(tim * DX_PI_F) * Config::Boss::Jump::JUMP_HEIGHT;
		}
		else
		{
			//着地・範囲判定
			comp.pos.x = landPos.x;
			comp.pos.z = landPos.z;
			comp.pos.y = jumpStartY;

			//攻撃判定
				Player& pl = boss.GetPlayer();
				VECTOR dir = VSub(pl.GetPosition(), comp.pos);
				dir.y = 0.0f;
				const float dist = VSize(dir);

				//プレイヤーが空中なら回避成功
				const bool playerAirborne = (pl.GetPosition().y > jumpStartY + 50.0f);

				if (dist <= Config::Boss::Jump::LAND_RADIUS && !playerAirborne)
				{
					pl.TakeDamage(Config::Boss::Jump::POWER, comp.pos);
				}
			

			//カメラシェイク
			boss.Camera().AddShake(15.0f);

			VECTOR center = comp.pos;
			const float groundY = boss.FloorYAt(comp.pos);
			boss.GetRockRing().Trigger(center, Config::Boss::Jump::LAND_RADIUS, groundY);

			VECTOR slamPos = comp.pos;
			slamPos.y = boss.FloorYAt(comp.pos);   
			slamPos.y = slamPos.y + 10;
			EffectManager::Instance().Play(ResourceManager::Instance().Effect("boss_slam"),slamPos,Config::Effect::BOSS_SLAM_SCALE);

			boss.Warning().Stop();
			step = Step::Done;
		}
		break;
	}
	case Step::Done:
	{
		return false;
	}
		
	}

	return true;
}

void BossAttackJump::OnEnd(Boss& boss)
{
	boss.SetAttackActive(false);
}

bool BossAttackJump::IsUsable(const Boss& boss) const
{
	/*
	* const float dist = boss.DistanceToPlayer();
	return dist >= Config::Boss::ATTACK_RANGE;  
	*/

	return true;
}

bool BossAttackJump::IsJustDodgeWindow(const Boss& boss) const
{
	if (step != Step::Jump)
	{
		return false;
	}

	const float t = timer / Config::Boss::Jump::JUMP_TIME;
	if (t < 0.9f)
	{
		return false;
	}


	//プレイヤーが着地範囲内にいるときだけ受付
	{
		VECTOR d = VSub(boss.GetPlayer().GetPosition(), landPos);
		d.y = 0.0f;
		return VSize(d) <= Config::Boss::Jump::LAND_RADIUS;
	}
	

	return false;
}
