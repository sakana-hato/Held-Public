#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "Stage.h"
#include "EnemyState.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "SharedContext.h"
#include "Player.h"

void EnemyIdle::OnEnter()
{
	const int idle = ResourceManager::Instance().Model("enemy_idle");
	if (idle >= 0)
	{
		enemy.PlayAnim(idle, 0, true);
	}

}

void EnemyIdle::Update(float dt)
{
	//探知円にプレイヤーが入ったら追跡へ
	if (enemy.DistanceToPlayer() <= Config::Enemy::DETECT_RANGE)
	{
		enemy.ChangeState(EnemyStateId::Chase);
	}
}

void EnemyChase::OnEnter()
{
	const int walk = ResourceManager::Instance().Model("enemy_walk");

	if (walk >= 0)
	{
		enemy.PlayAnim(walk, 0, true);
	}

}

void EnemyChase::Update(float dt)
{
	const float dist = enemy.DistanceToPlayer();

	//探知円から出たら待機へ戻る
	if (dist > Config::Enemy::DETECT_RANGE)
	{
		enemy.ChangeState(EnemyStateId::Idle);
		return;
	}

	//攻撃範囲に入ったら攻撃
	if (dist <= Config::Enemy::ATTACK_RANGE && enemy.GetAttackCooldown() <= 0.0f)
	{
		enemy.ChangeState(EnemyStateId::Attack);
		return;
	}

	//プレイヤーへ近づく
	enemy.MoveTowardPlayer(Config::Enemy::MOVE_SPEED, dt);
}

void EnemyAttack::OnEnter()
{
	timer = 0.0f;
	hitDone = false;
	enemy.SetAttackActive(false);

	//攻撃の瞬間はプレイヤーの方を向いておく
	enemy.FaceTowardDeg(enemy.YawToPlayerDeg(), 1.0f);

	const int atk = ResourceManager::Instance().Model("enemy_attack");
	if (atk >= 0)
	{
		enemy.PlayAnim(atk, 0, false);
	}

}

void EnemyAttack::Update(float dt)
{
	timer += dt;

	//攻撃判定の有効時間帯だけ手カプセルをON
	const bool active =
		(timer >= Config::Enemy::ATTACK_HIT_START &&
			timer <= Config::Enemy::ATTACK_HIT_END);
	enemy.SetAttackActive(active);

	//プレイヤーへのヒット判定
	if (active && !hitDone)
	{
		const Capsule atk = enemy.GetAttackCapsule();
		const Capsule body = enemy.GetPlayer().GetBodyCapsule();
		if (CapsuleMath::Intersect(atk, body))
		{
			enemy.GetPlayer().TakeDamage(Config::Enemy::ENEMY_ATTACK_POWER, enemy.Comp().pos);
			hitDone = true;
		}
	}

	//攻撃終了
	if (timer >= Config::Enemy::ATTACK_DURATION)
	{
		enemy.SetAttackActive(false);
		enemy.SetAttackCooldown(Config::Enemy::ATTACK_COOLDOWN);
		enemy.ChangeState(EnemyStateId::Chase);
	}
}

void EnemyDamage::OnEnter()
{
	timer = 0.0f;
	enemy.SetAttackActive(false);

	const int dmg = ResourceManager::Instance().Model("enemy_damaged");
	if (dmg >= 0)
	{
		enemy.PlayAnim(dmg, 0, false);
	}
	enemy.Comp().velocity = VScale(knockDir, Config::Enemy::DAMAGED_KNOCKBACK);
}

void EnemyDamage::Update(float dt)
{
	timer += dt;

	auto& comp = enemy.Comp();

	//のけぞり移動
	const float t = 1.0f - (timer / Config::Enemy::DAMAGED_DURATION);
	if (t > 0.0f)
	{
		comp.pos.x += knockDir.x * Config::Enemy::DAMAGED_KNOCKBACK * t * dt;
		comp.pos.z += knockDir.z * Config::Enemy::DAMAGED_KNOCKBACK * t * dt;

		//壁・柱で止める
		comp.pos = enemy.GetStage().ResolveWall(comp.pos, comp.radius, comp.height);
	}

	if (timer >= Config::Enemy::DAMAGED_DURATION)
	{
		enemy.ChangeState(EnemyStateId::Chase);
	}
}

void EnemyDead::OnEnter()
{
	timer = 0.0f;
	enemy.SetAttackActive(false);

	const int dead = ResourceManager::Instance().Model("enemy_dead");
	if (dead >= 0)
	{
		enemy.PlayAnim(dead, 0, false);
		enemy.Anim().SetSpeed(Config::Enemy::DEAD_ANIM_SPEED);
	}

	vy = Config::Enemy::DEAD_LAUNCH_VY;

}

void EnemyDead::Update(float dt)
{
	timer += dt;
	auto& comp = enemy.Comp();

	//水平に吹き飛ぶ（プレイヤーが向いている方向＝knockDir）
	comp.pos.x += knockDir.x * Config::Enemy::DEAD_LAUNCH_SPEED * dt;
	comp.pos.z += knockDir.z * Config::Enemy::DEAD_LAUNCH_SPEED * dt;

	//垂直方向：重力で放物線を描く
	vy -= Config::Player::Status::GRAVITY * dt;
	comp.pos.y += vy * dt;

	//地面に着いたら止める
	const float floorY = enemy.FloorYAt(comp.pos);
	if (comp.pos.y <= floorY)
	{
		comp.pos.y = floorY;
		vy = 0.0f;
	}

	//壁で止める（水平方向）
	comp.pos = enemy.GetStage().ResolveWall(comp.pos, comp.radius, comp.height);

	//死亡演出が終わったら EnemyManager に消してもらう
	if (timer >= Config::Enemy::DEAD_DURATION)
	{
		enemy.SetWantsRemove();
	}
}