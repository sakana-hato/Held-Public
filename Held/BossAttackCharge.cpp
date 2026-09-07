#include "DxLib.h"
#include "BossAttackCharge.h"
#include "Config.h"
#include "Boss.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"
#include "EffectManager.h"

void BossAttackCharge::OnStart(Boss& boss)
{
	step = Step::MoveToOrbit;
	timer = 0.0f;
	dashDone = 0;
	orbitProgress = 0.0f;
	afterImages_.clear();
	afterImageTimer = 0.0f;
	boss.SetHitPlayer(false);

	//外周の中心＝アリーナ中心
	orbitCenter = VGet(Config::Boss::ARENA_CENTER_X, boss.Comp().pos.y, Config::Boss::ARENA_CENTER_Z);

	//今のボス位置から、外周上の開始角度を決める
	VECTOR toBoss = VSub(boss.Comp().pos, orbitCenter);
	orbitAngle = atan2f(toBoss.z, toBoss.x);

	//走りアニメ
	const int run = ResourceManager::Instance().Model("boss_run");
	if (run >= 0)
	{
		boss.PlayAnim(run, 0, true);
	}

	const float yaw = boss.Comp().facingYawDeg * DX_PI_F / 180.0f;
	VECTOR effectPos = boss.Comp().pos;
	effectPos.y += Config::Boss::Charge::WIND_EFFECT_Y;
	chargeEffectInstance = EffectManager::Instance().Play(ResourceManager::Instance().Effect("charge_wind"),effectPos,Config::Effect::CHARGE_WIND_SCALE,VGet(0.0f, yaw, 0.0f));
}

bool BossAttackCharge::Update(Boss& boss, float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	//残像を一定間隔で記録
	afterImageTimer -= dt;
	if (afterImageTimer <= 0.0f)
	{
		afterImageTimer = Config::Boss::Charge::AFTERIMAGE_INTERVAL;

		AfterImage ai;
		ai.pos = comp.pos;
		ai.yawDeg = comp.facingYawDeg;
		ai.life = 0.0f;
		ai.animModel = boss.Anim().CurrentAnimModel();
		ai.animIndex = boss.Anim().CurrentAnimIndex();
		ai.animTime = boss.Anim().CurrentAnimTime();
		afterImages_.push_back(ai);
	}
	//残像の寿命
	for (auto& ai : afterImages_)
	{
		ai.life += dt;
	}

	afterImages_.erase(std::remove_if(afterImages_.begin(), afterImages_.end(),[](const AfterImage& a) { return a.life > Config::Boss::Charge::AFTERIMAGE_LIFE; }),afterImages_.end());

	if (chargeEffectInstance < 0 || !EffectManager::Instance().IsPlaying(chargeEffectInstance))
	{
		//消えているので、また出す
		const float yaw = comp.facingYawDeg * DX_PI_F / 180.0f;
		VECTOR effectPos = comp.pos;
		effectPos.y += Config::Boss::Charge::WIND_EFFECT_Y;
		VECTOR forward = comp.Forward();
		effectPos.x += forward.x * Config::Boss::Charge::WIND_EFFECT_FORWARD;
		effectPos.z += forward.z * Config::Boss::Charge::WIND_EFFECT_FORWARD;

		chargeEffectInstance = EffectManager::Instance().Play(
			ResourceManager::Instance().Effect("charge_wind"),
			effectPos,
			Config::Effect::CHARGE_WIND_SCALE,
			VGet(0.0f, yaw, 0.0f));
	}
	else
	{
		//再生中なので、位置・向き・速度を更新（追従）
		const float yaw = comp.facingYawDeg * DX_PI_F / 180.0f;
		VECTOR effectPos = comp.pos;
		effectPos.y += Config::Boss::Charge::WIND_EFFECT_Y;
		VECTOR forward = comp.Forward();
		effectPos.x += forward.x * Config::Boss::Charge::WIND_EFFECT_FORWARD;
		effectPos.z += forward.z * Config::Boss::Charge::WIND_EFFECT_FORWARD;

		EffectManager::Instance().SetPosition(chargeEffectInstance, effectPos);
		EffectManager::Instance().SetRotation(chargeEffectInstance, VGet(0.0f, yaw, 0.0f));

		//ジャスト回避スロー中は遅く
		float effectSpeed = 1.0f;
		if (boss.GetPlayer().IsSlowMoActive())
		{
			effectSpeed = Config::PostEffect::SLOWMO_SCALE;
		}
		EffectManager::Instance().SetSpeed(chargeEffectInstance, effectSpeed);
	}

	switch (step)
	{
	case Step::MoveToOrbit:
	{
		//現在位置から、一番近い外周上の点へ走る
		VECTOR fromCenter = VSub(comp.pos, orbitCenter);
		fromCenter.y = 0.0f;
		const float distFromCenter = VSize(fromCenter);

		if (distFromCenter > 1e-4f)
		{
			VECTOR dir = VScale(fromCenter, 1.0f / distFromCenter);
			//外周上の目標点
			const VECTOR target = VGet(
				orbitCenter.x + dir.x * Config::Boss::Charge::ORBIT_RADIUS,
				comp.pos.y,
				orbitCenter.z + dir.z * Config::Boss::Charge::ORBIT_RADIUS);

			//目標へ走る
			VECTOR to = VSub(target, comp.pos);
			to.y = 0.0f;
			const float toLen = VSize(to);

			if (toLen <= 50.0f)   //外周に着いた
			{
				//外周の角度を、今の位置から設定して円運動へ
				comp.pos.x = target.x;
				comp.pos.z = target.z;
				orbitAngle = atan2f(dir.z, dir.x);
				step = Step::Orbit;
				timer = 0.0f;
				orbitProgress = 0.0f;
			}
			else
			{
				//外周へ向かって走る
				VECTOR moveDir = VScale(to, 1.0f / toLen);
				comp.pos.x += moveDir.x * Config::Boss::Charge::ORBIT_SPEED * dt;
				comp.pos.z += moveDir.z * Config::Boss::Charge::ORBIT_SPEED * dt;

				//進行方向を向く
				const float moveYaw = atan2f(moveDir.x, moveDir.z) * 180.0f / DX_PI_F;
				boss.FaceTowardDeg(moveYaw, dt * 6.0f);
			}
		}
		break;
	}
	case Step::Orbit:
	{
		//外周を円を描いて走る（角度を滑らかに進める）
		const float angularSpeed = Config::Boss::Charge::ORBIT_SPEED / Config::Boss::Charge::ORBIT_RADIUS;
		orbitAngle += angularSpeed * dt;
		orbitProgress += angularSpeed * dt;

		//外周上の位置
		comp.pos.x = orbitCenter.x + cosf(orbitAngle) * Config::Boss::Charge::ORBIT_RADIUS;
		comp.pos.z = orbitCenter.z + sinf(orbitAngle) * Config::Boss::Charge::ORBIT_RADIUS;

		//進行方向（接線）を向く
		VECTOR tangent = VGet(-sinf(orbitAngle), 0.0f, cosf(orbitAngle));
		const float moveYaw = atan2f(tangent.x, tangent.z) * 180.0f / DX_PI_F;
		boss.FaceTowardDeg(moveYaw, dt * 8.0f);

		//ある程度外周を進んだら、プレイヤーへ突進
		if (orbitProgress >= Config::Boss::Charge::DASH_TRIGGER)
		{
			step = Step::Aim;
			timer = 0.0f;
			orbitProgress = 0.0f;
			dashDir = boss.DirToPlayer();   //突進方向を固定
		}
		break;
	}
	case Step::Aim:
	{
		dashDir = boss.DirToPlayer();
		const float aimYaw = atan2f(dashDir.x, dashDir.z) * 180.0f / DX_PI_F;
		boss.FaceTowardDeg(aimYaw, dt * 8.0f);

		//警告の線（ボスから突進方向へ）
		aimStart = comp.pos;
		aimEnd = VAdd(aimStart, VScale(dashDir, Config::Boss::Charge::DASH_LENGTH));

		if (timer >= Config::Boss::Charge::AIM_TIME)
		{
			step = Step::Dash;
			timer = 0.0f;

			//dashWentInside = false;

		}
		break;
	}
	case Step::Dash:
	{
		//突進（円の内側へ突っ込む）
		comp.pos.x += dashDir.x * Config::Boss::Charge::DASH_SPEED * dt;
		comp.pos.z += dashDir.z * Config::Boss::Charge::DASH_SPEED * dt;

		const float dashYaw = atan2f(dashDir.x, dashDir.z) * 180.0f / DX_PI_F;
		boss.FaceTowardDeg(dashYaw, dt * 10.0f);
		boss.SetAttackActive(true);

		//円の反対側の外周に到達したかを見る
		VECTOR fromCenter = VSub(comp.pos, orbitCenter);
		fromCenter.y = 0.0f;
		const float distFromCenter = VSize(fromCenter);

		//外周半径を超えたら（円を突き抜けて反対側の外周に出たら）
		if (distFromCenter >= Config::Boss::Charge::ORBIT_RADIUS)
		{
			boss.SetAttackActive(false);
			boss.SetHitPlayer(false);
			dashDone++;

			

			//外周上の位置に固定（今の方向で、半径ちょうどの位置に置く）
			if (distFromCenter > 1e-4f)
			{
				VECTOR dir = VScale(fromCenter, 1.0f / distFromCenter);
				comp.pos.x = orbitCenter.x + dir.x * Config::Boss::Charge::ORBIT_RADIUS;
				comp.pos.z = orbitCenter.z + dir.z * Config::Boss::Charge::ORBIT_RADIUS;
				//その位置の角度を、外周の角度として設定
				orbitAngle = atan2f(dir.z, dir.x);
			}

			if (dashDone >= Config::Boss::Charge::DASH_COUNT)
			{
				step = Step::Done;
			}
			else
			{
				step = Step::Orbit;
				timer = 0.0f;
				orbitProgress = 0.0f;
			}
		}
		break;
	}
	case Step::Done:
		boss.SetAttackActive(false);
		return false;
	}

	return true;
}

void BossAttackCharge::OnEnd(Boss& boss)
{
	boss.SetAttackActive(false);
	afterImages_.clear();

	if (chargeEffectInstance >= 0)
	{
		EffectManager::Instance().Stop(chargeEffectInstance);
		chargeEffectInstance = -1;
	}
}

bool BossAttackCharge::IsUsable(const Boss& boss) const
{
	return true;
}