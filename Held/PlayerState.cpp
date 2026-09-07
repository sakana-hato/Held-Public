#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "InputSystem.h"
#include "Stage.h"
#include "PlayerState.h"
#include "Player.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "EnemyManager.h"  
#include "Enemy.h"          
#include "Boss.h"
#include "EffectManager.h"
#include "TargetSystem.h"

namespace
{
	//ワールド方向(+Z基準)から yaw 角(度)を求める
	float DirToYawDeg(const VECTOR& dir)
	{
		return std::atan2(dir.x, dir.z) * 180.0f / DX_PI_F;
	}

	bool FindHomingTarget(Player& player, float range, VECTOR& outPos)
	{
		float bestDist = range;
		bool found = false;
		VECTOR result = VGet(0.0f, 0.0f, 0.0f);

		//雑魚から最寄り
		if (player.GetEnemies())
		{
			Enemy* ene = player.GetEnemies()->FindNearest(player.GetPosition(), range);
			if (ene)
			{
				VECTOR dir = VSub(ene->Comp().pos, player.GetPosition());
				dir.y = 0.0f;
				const float dist = VSize(dir);
				if (dist < bestDist)
				{
					bestDist = dist;
					result = ene->Comp().pos;
					found = true;
				}
			}
		}

		//ボスも候補
		Boss* bos = player.GetBoss();
		if (bos && bos->IsActive() && bos->IsAlive())
		{
			VECTOR dir = VSub(bos->Comp().pos, player.GetPosition());
			dir.y = 0.0f;
			const float dist = VSize(dir);
			//ボス用の範囲内で、かつ今の候補より近ければ
			if (dist <= Config::Player::Attack::ATTACK_HOMING_RANGE_BOSS && dist < bestDist)
			{
				bestDist = dist;
				result = bos->Comp().pos;
				found = true;
			}
		}

		if (found)
		{
			outPos = result;
		}
		return found;
	}
}

bool PlayerState::MoveByInput(float speed, float dt)
{
	const VECTOR dir = player.CalcMoveDirFromInput();
	if (VSize(dir) <= 0.0f)
	{
		return false;
	}


	player.FaceTowardDeg(DirToYawDeg(dir), dt);
	player.ApplyHorizontalMove(dir, speed, dt);
	return true;
}

void BaseMovement::ResetIdleTrigger()
{
	const float range = Config::Player::Idle::IDLE_ACT_WAIT_MAX - Config::Player::Idle::IDLE_ACT_WAIT_MIN;
	const float rand = static_cast<float>(GetRand(1000)) / 1000.0f;  
	idleTrigger = Config::Player::Idle::IDLE_ACT_WAIT_MIN + range * rand;
}

void BaseMovement::OnEnter()
{
	player.Comp().comboIndex = 0;
	wasDashMoving	= false;
	runStopping		= false;
	runStopTimer	= 0.0f;
	runToggled		= false;

	idleTimer		= 0.0f;
	idleActing		= false;
	ResetIdleTrigger();  
}

void BaseMovement::PlayIdle()
{
	const char* key = player.IsKatanaDrawn() ? "anim_idle_katana" : "anim_idle";
	const int idle = ResourceManager::Instance().Model(key);
	player.PlayAnim(idle, 0, true, false);
}

void BaseMovement::Update(float dt)
{
	auto& in		= player.Input();
	auto& comp		= player.Comp();

	//通常の無敵は無し（ジャスト回避スロー中だけ維持）
	comp.invincible = player.IsSlowMoActive();

	if (player.IsDrawingKatana())
	{
		
		return;
	}

	//アクション遷移
	if (in.IsPressed(InputAction::Ultimate) && comp.ultGauge >= Config::Player::Ult::ULT_GAUGE_MAX)
	{
		player.ChangeState(PlayerStateId::Ultimate); 
		return;
	}
	if (in.IsPressed(InputAction::Magic))
	{
		player.ChangeState(PlayerStateId::Magic); 
		return;
	}
	if (in.IsPressed(InputAction::Attack))
	{
		if (player.IsKatanaDrawn())       
		{
			if (player.IsSlowMoActive() && !player.IsCounterDashUsed())
			{
				player.MarkCounterDashUsed();
				player.ChangeState(PlayerStateId::CounterDash);
				return;
			}

			comp.comboIndex = 0;
			player.ChangeState(PlayerStateId::Attack);
			return;
		}
	}
	if (in.IsPressed(InputAction::Dodge))
	{
		player.ChangeState(PlayerStateId::Dodge);
		return;
	}
	if (in.IsPressed(InputAction::Jump))
	{
		comp.vy = Config::Player::Move::PLAYER_JUMP_SPEED;
		player.ChangeState(PlayerStateId::Jump); 
		return;
	}

	const VECTOR dir = player.CalcMoveDirFromInput();
	const bool   moving = (VSize(dir) > 0.0f);

	if (in.IsPressed(InputAction::Dash))
	{
		comp.dashOn = !comp.dashOn;
	}
	const bool dashing = comp.dashOn;

	if (runStopping)
	{
		//移動入力が入ったらキャンセルして通常移動へ戻す
		if (moving)
		{
			runStopping = false;
			runStopTimer = 0.0f;
			idleActing = false;
			idleTimer = 0.0f;   
		}
		else
		{
			//runstop を流し続ける。終わったら idle 
			runStopTimer += dt;
			comp.velocity = VGet(0.0f, 0.0f, 0.0f);

			

			const int runstop = ResourceManager::Instance().Model("anim_runstop");
			player.PlayAnim(runstop, 0, false, false); 

			if (runStopTimer >= Config::Player::Move::RUNSTOP_DURATION)
			{
		
				runStopping = false;
				runStopTimer = 0.0f;
				PlayIdle();
			}
			wasDashMoving = false;
			return;
		}
	}

	if (moving)
	{
		idleActing = false;
		idleTimer = 0.0f;
		const float speed = dashing ? Config::Player::Move::PLAYER_DASH_SPEED : Config::Player::Move::PLAYER_MOVE_SPEED;
		

		VECTOR targetPos;
		const bool locked = player.GetTarget().GetTargetPosition(targetPos);

		if (locked)
		{
			//対象を向いたまま入力方向へ移動（雑魚・ボス共通）
			VECTOR toTarget = VSub(targetPos, comp.pos);
			toTarget.y = 0.0f;
			if (VSize(toTarget) > 1e-4f)
			{
				player.FaceTowardDeg(DirToYawDeg(toTarget), dt);
			}

			player.ApplyHorizontalMove(dir, speed, dt);

			const float faceYaw = comp.facingYawDeg;
			const float moveYaw = DirToYawDeg(dir);
			float diff = moveYaw - faceYaw;
			while (diff > 180.0f)
			{
				diff -= 360.0f;
			}

			while (diff < -180.0f)
			{
				diff += 360.0f;
			}

			if (diff > -45.0f && diff <= 45.0f)
			{
				//前後ろ移動
				const char* key = dashing ? "anim_run" : "anim_walk";
				const int mv = ResourceManager::Instance().Model(key);
				if (mv >= 0)
				{
					player.PlayAnim(mv, 0, true, false);
				}

			}
			else if (diff > 45.0f && diff <= 135.0f)
			{
				//右横移動
				const int mv = ResourceManager::Instance().Model("anim_strafe_right");
				if (mv >= 0)
				{
					player.PlayAnim(mv, 0, true, false);
				}

			}
			else if (diff > -135.0f && diff <= -45.0f)
			{
				//左横移動
				const int mv = ResourceManager::Instance().Model("anim_strafe_left");
				if (mv >= 0)
				{
					player.PlayAnim(mv, 0, true, false);
				}

			}
			else
			{
				//後退
				const int mv = ResourceManager::Instance().Model("anim_strafe_back");
				if (mv >= 0)
				{
					player.PlayAnim(mv, 0, true, false);
				}
			}

			wasDashMoving = dashing;
		}
		else
		{
			//通常移動
			player.FaceTowardDeg(DirToYawDeg(dir), dt);
			player.ApplyHorizontalMove(dir, speed, dt);

			if (dashing)
			{
				const int run = ResourceManager::Instance().Model("anim_run");
				player.PlayAnim(run, 0, true, false);
			}
			else
			{
				const int walk = ResourceManager::Instance().Model("anim_walk");
				player.PlayAnim(walk, 0, true, false);
			}

			wasDashMoving = dashing;
		}

	}
	else
	{
		comp.velocity = VGet(0.0f, 0.0f, 0.0f);

		comp.dashOn = false;

		if (wasDashMoving)
		{
			runStopping			= true;
			runStopTimer		= 0.0f;
			const int runstop	= ResourceManager::Instance().Model("anim_runstop");
			player.PlayAnim(runstop, 0, false, false);
			idleActing			= false;
			idleTimer			= 0.0f;
			ResetIdleTrigger();
		}
		else if (idleActing)
		{
			//待機モーション再生中：アニメが最後まで再生されたら通常idleへ
			if (player.Anim().IsFinished())
			{
				idleActing = false;
				idleTimer = 0.0f;
				ResetIdleTrigger();
				PlayIdle();
			}
		}
		else
		{
			//通常idle中：無操作時間を計測
			idleTimer += dt;

			if (idleTimer >= idleTrigger && !player.IsKatanaDrawn())
			{
				const int pick = GetRand(Config::Player::Idle::IDLE_ACT_COUNT - 1); 
				const char* key =
					(pick == 0) ? "anim_idle2_a" :(pick == 1) ? "anim_idle2_b" : "anim_idle2_c";

				const int act = ResourceManager::Instance().Model(key);
				if (act >= 0)
				{
					player.PlayAnim(act, 0, false, false);  
					idleActing = true;
				}
				else
				{
					ResetIdleTrigger();
					idleTimer = 0.0f;
				}
			}
			else
			{
				PlayIdle();
			}
		}

		wasDashMoving = false;
	}
}


void DodgeState::OnEnter()
{
	timer			= 0.0f;
	auto& comp		= player.Comp();
	comp.invincible	= true;

	//回避方向：入力があればその方向、無ければバックステップ
	VECTOR inputDir = player.CalcMoveDirFromInput();

	if (VSize(inputDir) > 0.0f)
	{
		//入力あり： その方向へ突進。向きも変える。走りに繋ぐ
		dir = inputDir;
		comp.facingYawDeg = DirToYawDeg(dir);   //突進方向を向く
		linkToRun = true;

		//共通の回避アニメ
		const int dodge = ResourceManager::Instance().Model("anim_dodge");
		player.PlayAnim(dodge, 0, false, false);

		VECTOR dummyTarget;
		const bool isTargeting = player.GetTargetSystem().GetTargetPosition(dummyTarget);

		if (!isTargeting)
		{
			const int fx = ResourceManager::Instance().Effect("dodge_wind");
			if (fx >= 0)
			{
				const VECTOR forward = comp.Forward();
				VECTOR fxPos = comp.pos;
				fxPos.x += forward.x * Config::Player::Evasion::DODGE_FX_FORWARD;
				fxPos.z += forward.z * Config::Player::Evasion::DODGE_FX_FORWARD;
				fxPos.y += Config::Player::Evasion::DODGE_FX_HEIGHT;

				const float yaw = comp.facingYawDeg * DX_PI_F / 180.0f;
				const int inst = EffectManager::Instance().Play(fx, fxPos, Config::Player::Evasion::DODGE_FX_SCALE);
				EffectManager::Instance().SetRotation(inst, VGet(0.0f, yaw, 0.0f));
			}
		}
	}
	else
	{
		//後ろへ回避。向きは変えない
		dir = VScale(comp.Forward(), -1.0f);
		linkToRun = false;

		//後ろ回避専用アニメ
		const int backDodge = ResourceManager::Instance().Model("anim_dodge_back");
		player.PlayAnim(backDodge, 0, false, false);
		player.Anim().SetSpeed(Config::Player::Evasion::BACK_DODGE_ANIM_SPEED);
	}

	

	//comp.facingYawDeg = DirToYawDeg(dir);


	if (player.IsIncomingAttack())
	{
		player.TriggerJustDodgeSuccess();
	}
}

void DodgeState::Update(float dt)
{
	timer += dt;
	auto& comp = player.Comp();

	const bool dashing = (timer < Config::Player::Evasion::DODGE_DURATION);
	if (dashing)
	{
		//この間に敵攻撃が来たらジャスト回避成功
		if (player.IsIncomingAttack() && !player.IsSlowMoActive())
		{
			player.TriggerJustDodgeSuccess();
		}
	}


	//回避の無敵終了（ただしジャスト回避スロー中は維持）
	if (timer >= Config::Player::Evasion::DODGE_INVINCIBLE && !player.IsSlowMoActive())
	{
		comp.invincible = false;
	}

	//だんだん減速しながら移動
	const float t = timer / Config::Player::Evasion::DODGE_DURATION;
	const float speed = Config::Player::Evasion::DODGE_SPEED * (1.0f - t);
	player.ApplyHorizontalMove(dir, (speed > 0.0f) ? speed : 0.0f, dt);

	if (timer >= Config::Player::Evasion::DODGE_DURATION)
	{
		if (linkToRun)
		{
			comp.dashOn = true;
			
		}
		player.ChangeState(PlayerStateId::BaseMovement);
		/*
		* else
		{
			player.ChangeState(PlayerStateId::BaseMovement);
		}
		*/
		
	}
}

void DodgeState::OnExit()
{
	player.SetJustDodgeWindow(false);
}

void CounterDashState::OnEnter()
{
	timer = 0.0f;
	hitDoneCount = 0;
	hasTarget = false;

	auto& comp = player.Comp();
	auto& ctx = player.Ctx();

	//無敵にする（演出中は被弾しない）
	comp.invincible = true;
	comp.velocity = VGet(0.0f, 0.0f, 0.0f);

	//プレイヤーを非表示に
	player.SetHidden(true);

	//---- 一番近い敵（雑魚＋ボス）を探して targetCenter を決める ----
	float bestDistSq = Config::Player::Counter::SEARCH_MAX * Config::Player::Counter::SEARCH_MAX;

	if (player.GetEnemies())
	{
		Enemy* nearest = player.GetEnemies()->FindNearest(comp.pos, Config::Player::Counter::SEARCH_MAX);
		if (nearest)
		{
			const VECTOR ep = nearest->Comp().pos;
			VECTOR d = VSub(ep, comp.pos); d.y = 0.0f;
			const float sq = d.x * d.x + d.z * d.z;
			if (sq < bestDistSq)
			{
				bestDistSq = sq;
				targetCenter = ep;
				hasTarget = true;
			}
		}
	}

	if (player.GetBoss() && player.GetBoss()->IsActive() && player.GetBoss()->IsAlive())
	{
		const VECTOR bp = player.GetBoss()->Comp().pos;
		VECTOR d = VSub(bp, comp.pos); d.y = 0.0f;
		const float sq = d.x * d.x + d.z * d.z;
		if (sq < bestDistSq)
		{
			bestDistSq = sq;
			targetCenter = bp;
			hasTarget = true;
		}
	}

	if (!hasTarget)
	{
		//敵がいなければ、その場でエフェクトを出すだけ
		targetCenter = comp.pos;
	}

	if (hasTarget)
	{
		VECTOR toTarget = VSub(targetCenter, comp.pos);
		toTarget.y = 0.0f;
		const float d = VSize(toTarget);

		if (d > 1e-3f)
		{
			VECTOR dir = VScale(toTarget, 1.0f / d);
			VECTOR newPos = targetCenter;
			newPos.x = targetCenter.x - dir.x * Config::Player::Counter::FINISH_OFFSET;
			newPos.z = targetCenter.z - dir.z * Config::Player::Counter::FINISH_OFFSET;
			newPos.y = player.FloorYAt(newPos);
			comp.pos = newPos;

			//向きを敵に向ける
			const float yawDeg = std::atan2(dir.x, dir.z) * 180.0f / DX_PI_F;
			comp.facingYawDeg = yawDeg;
		}
	}

}

void CounterDashState::Update(float dt)
{
	timer += dt;

	//---- ダメージのタイミング（一定間隔で HIT_COUNT 回）----
	const float interval = Config::Player::Counter::HIT_INTERVAL;
	const int   maxHits = Config::Player::Counter::HIT_COUNT;

	//timer が interval * (hitDoneCount + 1) を超えたら、次のダメージを入れる
	while (hitDoneCount < maxHits && timer >= interval * (hitDoneCount + 1))
	{
		ApplyRushDamage();
		hitDoneCount++;
	}

	//---- 5撃終わって、少しの余韻の後にプレイヤーを敵の近くに出現させる ----
	const float endTime = interval * maxHits;

	if (hitDoneCount >= maxHits && timer >= endTime)
	{
		//表示に戻す
		player.SetHidden(false);

		player.ChangeState(PlayerStateId::BaseMovement);
		return;
	}
}

void CounterDashState::OnExit()
{
	player.Comp().invincible = false;
	player.SetHidden(false);   //念のため表示に戻す
}


static void CounterDashState_ApplyRushDamageImpl(Player& player, const VECTOR& center)
{
	auto& ctx = player.Ctx();
	const float radius = Config::Player::Counter::HIT_RADIUS;
	const float radSq = radius * radius;

	//ダメージ量：通常の攻撃力に HIT_DAMAGE_MUL を掛ける
	const float damage = player.GetCurrentAttackPower() * Config::Player::Counter::HIT_DAMAGE_MUL;

	//---- 範囲内の雑魚敵にダメージ ----
	if (player.GetEnemies())
	{
		std::vector<Enemy*> list;
		player.GetEnemies()->AliveEnemyList(list);
		for (Enemy* e : list)
		{
			VECTOR d = VSub(e->Comp().pos, center); d.y = 0.0f;
			const float sq = d.x * d.x + d.z * d.z;
			if (sq <= radSq)
			{
				e->TakeDamage(damage, center);
			}
		}
	}

	//---- 範囲内ならボスにもダメージ ----
	Boss* bos = player.GetBoss();
	if (bos && bos->IsActive() && bos->IsAlive())
	{
		VECTOR d = VSub(bos->Comp().pos, center); d.y = 0.0f;
		const float sq = d.x * d.x + d.z * d.z;
		if (sq <= radSq)
		{
			bos->TakeDamage(damage, center);
		}
	}
}

//CounterDashState のメンバから呼ぶ
void CounterDashState::ApplyRushDamage()
{
	auto& ctx = player.Ctx();
	const float radius = Config::Player::Counter::HIT_RADIUS;
	const float radSq = radius * radius;
	const float damage = player.GetCurrentAttackPower() * Config::Player::Counter::HIT_DAMAGE_MUL;

	const int hitFx			= ResourceManager::Instance().Effect("hit_slash");
	const float hitScale = Config::Player::Counter::HIT_FX_SCALE;

	//範囲内の雑魚敵にダメージ
	if (player.GetEnemies())
	{
		std::vector<Enemy*> list;
		player.GetEnemies()->AliveEnemyList(list);
		for (Enemy* e : list)
		{
			VECTOR d = VSub(e->Comp().pos, targetCenter); d.y = 0.0f;
			const float sq = d.x * d.x + d.z * d.z;
			if (sq <= radSq)
			{
				e->TakeDamage(damage, targetCenter);

				//ヒットエフェクトを敵の位置で
				if (hitFx >= 0)
				{
					VECTOR hitPos = e->Comp().pos;
					hitPos.y += 100.0f;   //少し上（腰あたり）に出す
					EffectManager::Instance().Play(hitFx, hitPos, hitScale);
				}
			}
		}
	}

	//範囲内ならボスにもダメージ
	Boss* bos = player.GetBoss();
	if (bos && bos->IsActive() && bos->IsAlive())
	{
		VECTOR d = VSub(bos->Comp().pos, targetCenter); d.y = 0.0f;
		const float sq = d.x * d.x + d.z * d.z;
		if (sq <= radSq)
		{
			bos->TakeDamage(damage, targetCenter);
			//ボスにもヒットエフェクト
			if (hitFx >= 0)
			{
				VECTOR hitPos = bos->Comp().pos;
				hitPos.y += 100.0f;
				EffectManager::Instance().Play(hitFx, hitPos, hitScale);
			}
		}
	}

	//エフェクトを再生
	//hitDoneCount がこの時点で「今回のヒットが何撃目か（0から数える）」を表す
	const bool useA = (hitDoneCount % 2 == 0);   //0,2,4撃目はA、1,3撃目はB

	const int fxA = ResourceManager::Instance().Effect("counter_rush_a");
	if (fxA >= 0)
	{
		EffectManager::Instance().Play(fxA, targetCenter, Config::Player::Counter::EFFECT_A_SCALE);
	}

	const int fxB = ResourceManager::Instance().Effect("counter_rush_b");
	if (fxB >= 0)
	{
		EffectManager::Instance().Play(fxB, targetCenter, Config::Player::Counter::EFFECT_B_SCALE);
	}
}



void JumpState::OnEnter()
{
	const int jump = ResourceManager::Instance().Model("anim_jump");
	player.PlayAnim(jump, 0, false, false);

	VECTOR footPos = player.GetPosition();
	footPos.y = player.FloorYAt(footPos); 
	EffectManager::Instance().Play(ResourceManager::Instance().Effect("jump_dust"),footPos,Config::Effect::JUMP_DUST_SCALE);
}

void JumpState::Update(float dt)
{
	auto& in = player.Input();

	//抜刀中のみ落下攻撃が可能
	if (in.IsPressed(InputAction::Attack) && player.IsKatanaDrawn())
	{
		player.ChangeState(PlayerStateId::JumpAttack); return;
	}

	//空中制御（弱め）
	MoveByInput(Config::Player::Move::PLAYER_MOVE_SPEED * Config::Player::Move::AIR_CONTROL, dt);
	player.ApplyGravity(dt);

	if (player.IsGrounded())
	{
		player.ChangeState(PlayerStateId::BaseMovement);
	}
}

void JumpAttackState::OnEnter()
{
	landed = false;
	recovery = 0.0f;
	rising = true;        
	riseTimer = 0.0f;
	attackTimer = 0.0f;
	player.NextAttackId();

	//少し上に浮かせる初速（上向き）
	player.Comp().vy = Config::Player::Attack::JUMP_ATTACK_RISE_SPEED;  

	//吸い付き：近くの敵・ボスの方向を記録
	homingActive = false;
	VECTOR targetPos;
	if (FindHomingTarget(player, Config::Player::Attack::JUMP_ATTACK_HOMING_RANGE, targetPos))
	{
		VECTOR to = VSub(targetPos, player.GetPosition());
		to.y = 0.0f;
		const float len = VSize(to);
		if (len > 1e-4f)
		{
			homingDir = VScale(to, 1.0f / len);
			homingActive = true;
		}
	}


	//落下攻撃モーション
	const int anim = ResourceManager::Instance().Model("anim_jump_attack");
	if (anim >= 0)
	{
		player.PlayAnim(anim, 0, false, false);
		player.Anim().SetSpeed(Config::Player::Attack::JUMP_ATTACK_ANIM_SPEED);
	}

	//刀の攻撃判定ON
	player.SetBladeActive(false);
}

void JumpAttackState::Update(float dt)
{
	auto& comp = player.Comp();
	attackTimer += dt;

	const bool inHit =
		(attackTimer >= Config::Player::Attack::JUMP_ATTACK_HIT_START &&
			attackTimer <= Config::Player::Attack::JUMP_ATTACK_HIT_END);
	player.SetBladeActive(inHit);

	if (rising)
	{
		//上昇フェーズ：少し上に浮く
		riseTimer += dt;
		comp.pos.y += comp.vy * dt;
		comp.vy -= Config::Player::Attack::JUMP_ATTACK_RISE_GRAVITY * dt;   //だんだん減速

		//上昇時間が過ぎたら、落下に転じる
		if (riseTimer >= Config::Player::Attack::JUMP_ATTACK_RISE_TIME)
		{
			rising = false;
			comp.vy = -Config::Player::Attack::JUMP_ATTACK_FALL_SPEED;   //下向きに切り替え
		}
	}
	else if (!landed)
	{
		//落下フェーズ
		comp.pos.y += comp.vy * dt;

		if (homingActive)
		{
			//目標地点を毎フレーム更新（敵が動いても追う）
			VECTOR targetPos;
			if (FindHomingTarget(player, Config::Player::Attack::JUMP_ATTACK_HOMING_RANGE, targetPos))
			{
				VECTOR to = VSub(targetPos, comp.pos);
				to.y = 0.0f;
				const float len = VSize(to);
				if (len > 1e-4f)
				{
					VECTOR dir = VScale(to, 1.0f / len);

					//向きを徐々に敵へ（補間）
					const float targetYaw = std::atan2(dir.x, dir.z) * 180.0f / DX_PI_F;
					player.FaceTowardDeg(targetYaw, dt * Config::Player::Attack::JUMP_ATTACK_HOMING_TURN);

					//移動：距離に応じて速度を変える
					//補間的に寄る：残り距離の一定割合ずつ詰める
					const float t = Config::Player::Attack::JUMP_ATTACK_HOMING_LERP;
					comp.pos.x += (targetPos.x - comp.pos.x) * t * dt;
					comp.pos.z += (targetPos.z - comp.pos.z) * t * dt;
				}
			}
		}

		const float floorY = player.FloorYAt(comp.pos);
		if (comp.pos.y <= floorY)
		{
			comp.pos.y = floorY;
			comp.vy = 0.0f;
			landed = true;

			
			player.SetBladeActive(false);
			
			VECTOR landPos = comp.pos;
			landPos.y = floorY+10;   //地面の高さ
			EffectManager::Instance().Play(ResourceManager::Instance().Effect("jump_landing"),landPos,Config::Effect::JUMP_LANDING_SCALE);

		}
	}
	else
	{
		//着地後の硬直
		recovery += dt;
		if (recovery >= Config::Player::Attack::JUMP_ATTACK_RECOVERY)
		{
			player.ChangeState(PlayerStateId::BaseMovement);
		}
	}
}

void AttackState::OnEnter()
{
	timer = 0.0f;
	queued = false;
	player.NextAttackId();

	//向きの吸い付き
	if (player.GetEnemies() && Config::Player::Attack::ATTACK_HOMING_FACE > 0.0f)
	{
		VECTOR targetPos;
		if (FindHomingTarget(player, Config::Player::Attack::ATTACK_HOMING_RANGE, targetPos))
		{
			VECTOR to = VSub(targetPos, player.GetPosition());
			to.y = 0.0f;
			if (VSize(to) > 1e-4f)
			{
				const float enemyYaw = std::atan2(to.x, to.z) * 180.0f / DX_PI_F;
				//強度1なら即座に敵を向く。強度で現在向きと混ぜる。
				const float cur = player.Comp().facingYawDeg;
				float diff = enemyYaw - cur;
				while (diff > 180.0f)
				{
					diff -= 360.0f;
				}

				while (diff < -180.0f)
				{
					diff += 360.0f;
				}
				player.Comp().facingYawDeg = cur + diff * Config::Player::Attack::ATTACK_HOMING_FACE;
			}
		}
	}

	PlayAttackAnim();
}

void AttackState::OnExit()
{
	player.SetBladeActive(false);  
}

void AttackState::PlayAttackAnim()
{
	const int idx = player.Comp().comboIndex;  
	const char* key =(idx == 0) ? "anim_attack1" :(idx == 1) ? "anim_attack2" : "anim_attack3";

	const int anim = ResourceManager::Instance().Model(key);
	if (anim >= 0)
	{
		player.PlayAnim(anim, 0, false, false);
		player.Anim().SetSpeed(Config::Player::Attack::ATTACK_ANIM_SPEED);
	}
}

VECTOR AttackState::CalcAttackDir()
{
	const VECTOR forward = player.Comp().Forward();

	if (player.GetEnemies())
	{
		Enemy* target = player.GetEnemies()->FindNearest(
			player.GetPosition(), Config::Player::Attack::ATTACK_HOMING_RANGE);
		if (target)
		{
			VECTOR toEnemy = VSub(target->Comp().pos, player.GetPosition());
			toEnemy.y = 0.0f;
			const float len = VSize(toEnemy);
			if (len > 1e-4f)
			{
				toEnemy = VScale(toEnemy, 1.0f / len);
				//正面と敵方向を強度で混ぜる
				const float t = Config::Player::Attack::ATTACK_HOMING_MOVE;
				VECTOR mixed = VAdd(VScale(forward, 1.0f - t), VScale(toEnemy, t));
				const float ml = VSize(mixed);
				if (ml > 1e-4f)
				{
					return VScale(mixed, 1.0f / ml);
				}
			}
		}
	}
	return forward;
}

void AttackState::Update(float dt)
{
	auto& in = player.Input();
	auto& comp = player.Comp();
	timer += dt;

	float hitStart = Config::Player::Attack::ATTACK1_HIT_START;
	float hitEnd = Config::Player::Attack::ATTACK1_HIT_END;
	switch (comp.comboIndex)
	{
	case 0:
		hitStart = Config::Player::Attack::ATTACK1_HIT_START;
		hitEnd = Config::Player::Attack::ATTACK1_HIT_END;
		break;
	case 1:
		hitStart = Config::Player::Attack::ATTACK2_HIT_START;
		hitEnd = Config::Player::Attack::ATTACK2_HIT_END;
		break;
	case 2:
		hitStart = Config::Player::Attack::ATTACK3_HIT_START;
		hitEnd = Config::Player::Attack::ATTACK3_HIT_END;
		break;
	default:
		break;
	}

	const bool active = (timer >= hitStart && timer <= hitEnd);
	player.SetBladeActive(active);

	//次コンボの先行入力受付
	if (timer >= Config::Player::Attack::COMBO_INPUT_OPEN )
	{
		if (in.IsPressed(InputAction::Attack))
		{
			queued = true;
		}
	}

	//攻撃中の緩やかな前進
	if (timer <= Config::Player::Attack::ATTACK_STEP_TIME)
	{
		const float t = 1.0f - (timer / Config::Player::Attack::ATTACK_STEP_TIME);
		const float baseSpeed = (Config::Player::Attack::ATTACK_STEP_DISTANCE / Config::Player::Attack::ATTACK_STEP_TIME) * 2.0f;
		const float speed = baseSpeed * t;
		const VECTOR dir = CalcAttackDir();
		player.ApplyHorizontalMove(dir, speed, dt);
	}

	//回避でキャンセル可（アクション性UP）
	if (in.IsPressed(InputAction::Dodge))
	{
		player.ChangeState(PlayerStateId::Dodge); return;
	}

	if (player.Anim().IsFinished())
	{
		if (queued && (player.Comp().comboIndex + 1 < Config::Player::Attack::COMBO_MAX))
		{
			player.Comp().comboIndex++;
			timer = 0.0f;
			queued = false;
			player.NextAttackId();  
			PlayAttackAnim();
		}
		else
		{
			player.Comp().comboIndex = 0;
			player.ChangeState(PlayerStateId::BaseMovement);
		}
	}
}




void UltimateState::OnEnter()
{
	timer						= 0.0f;
	player.Comp().ultGauge		= 0.0f;   //消費
	player.Comp().invincible	= true; //演出中は無敵
	// TODO: 必殺技BGM／カメラ演出／炎をまとうエフェクト
}

void UltimateState::Update(float dt)
{
	timer += dt;
	player.ApplyHorizontalMove(player.Comp().Forward(), Config::Player::Ult::ULTIMATE_RUSH_SPEED, dt);

	if (timer >= Config::Player::Ult::ULTIMATE_DURATION)
	{
		player.Comp().invincible = false;
		player.ChangeState(PlayerStateId::BaseMovement);
	}
}

void DamagedState::OnEnter()
{
	timer = 0.0f;
	auto& comp = player.Comp();
	// TODO: のけぞりアニメ／ヒットストップ／被弾SE
	const int dmg = ResourceManager::Instance().Model("anim_damaged");
	if (dmg >= 0)
	{
		player.PlayAnim(dmg, 0, false, false);
	}
	comp.velocity = VScale(knockDir, Config::Player::Hit::DAMAGED_KNOCKBACK);
}

void DamagedState::Update(float dt)
{
	timer += dt;
	auto& comp = player.Comp();

	//のけぞり移動（だんだん減速）
	const float t = 1.0f - (timer / Config::Player::Hit::DAMAGED_DURATION);   //1→0
	if (t > 0.0f)
	{
		const VECTOR step = VScale(knockDir, Config::Player::Hit::DAMAGED_KNOCKBACK * t);
		comp.pos.x += step.x * dt;
		comp.pos.z += step.z * dt;
		//壁で止める
		comp.pos = player.GetStage().ResolveWall(comp.pos, comp.radius, comp.height);
	}

	//重力
	player.ApplyGravity(dt);

	//のけぞり終了→通常へ
	if (timer >= Config::Player::Hit::DAMAGED_DURATION)
	{
		player.ChangeState(PlayerStateId::BaseMovement);
	}

	

}

void DeadState::OnEnter()
{
	animFinished = false;
	auto& comp = player.Comp();
	comp.velocity = VGet(0.0f, 0.0f, 0.0f);   //動きを止める
	comp.invincible = true;                    //死亡後は無敵

	const int dead = ResourceManager::Instance().Model("anim_dead");
	if (dead >= 0)
	{
		player.PlayAnim(dead, 0, false, false);   //非ループ
	}
}

void DeadState::Update(float dt)
{
	//重力だけかけて地面に留まる（倒れposition保持）
	player.ApplyGravity(dt);

	//死亡アニメが終わったらフラグを立てる
	if (player.Anim().IsFinished())
	{
		animFinished = true;
	}
}