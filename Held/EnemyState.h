#pragma once
#include "State.h"
#include "EnemyStateId.h"

class Enemy;

/// <summary>
/// 雑魚敵の行動パターン
/// </summary>
class EnemyState :public state
{
public:
	explicit EnemyState(Enemy& enemy) : enemy(enemy) {}

protected:
	Enemy& enemy;
};

class EnemyIdle final :public EnemyState
{
public:
	using EnemyState::EnemyState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(EnemyStateId::Idle); }
};

class EnemyChase final : public EnemyState
{
public:
	using EnemyState::EnemyState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(EnemyStateId::Chase); }
};

class EnemyAttack final : public EnemyState
{
public:
	using EnemyState::EnemyState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(EnemyStateId::Attack); }
private:
	float timer = 0.0f;
	bool  hitDone = false;   //この攻撃で1回だけ判定を出す用
};

class EnemyDamage final : public EnemyState
{
public:
	using EnemyState::EnemyState;
	void OnEnter() override;
	void Update(float dt) override;
	void SetKnockbackDir(const VECTOR& dir) { knockDir = dir; }
	int  Id() const override { return static_cast<int>(EnemyStateId::Damage); }
private:
	float timer		= 0.0f;
	VECTOR knockDir = VGet(0.0f, 0.0f, 0.0f);
};

class EnemyDead final : public EnemyState
{
public:
	using EnemyState::EnemyState;
	void OnEnter() override;
	void Update(float dt) override;
	void SetKnockbackDir(const VECTOR dir) { knockDir = dir; }
	int  Id() const override { return static_cast<int>(EnemyStateId::Dead); }
private:
	float timer = 0.0f;
	VECTOR knockDir = VGet(0.0f, 0.0f, 0.0f);   //吹き飛ぶ水平方向
	float vy = 0.0f;                            //垂直速度（放物線用）
};
