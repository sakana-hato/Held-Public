#pragma once
#include "BossState.h"

/// <summary>
/// ボスの基本の移動処理をまとめたヘッダー
/// たぶん分けたほうが良い
/// </summary>

class BossIntroState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Intro); }
	bool IsFinished() const { return finished; }
private:
	float timer = 0.0f;
	bool  finished = false;
	float startY = 0.0f;
	float goalY = 0.0f;
};

class BossIdleState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Idle); }
private:
	float timer = 0.0f;
};

class BossChaseState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Chase); }
};

class BossAttackState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	void OnExit() override;
	int  Id() const override { return static_cast<int>(BossStateId::Attack); }
};

class BossDamageState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Damage); }
	void SetKnockbackDir(const VECTOR& dir) { knockDir = dir; }
private:
	VECTOR knockDir = VGet(0.0f, 0.0f, 0.0f);
	float  timer = 0.0f;
};

class BossStaggerState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Stagger); }
private:
	float timer = 0.0f;
};

class BossDeadState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::Dead); }
	bool IsAnimFinished() const { return animFinished; }
private:
	float timer = 0.0f;
	bool  animFinished = false;
};

class BossDrawSwordState final : public BossState
{
public:
	using BossState::BossState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(BossStateId::DrawSword); }
private:
	float timer = 0.0f;
	bool  roared = false;
	bool  drawn = false;
	bool  drawAnimStarted = false;
};
