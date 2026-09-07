#pragma once
#include "State.h"
#include "PlayerStateId.h"

class Player;//前方宣言

/// <summary>
/// プレイヤーの行動状態（State パターンの抽象クラス）
/// 各状態は Player の公開ヘルパーを通して移動・遷移を行う
/// </summary>
class PlayerState :public state
{
public:
	explicit PlayerState(Player& player) : player(player) {}

protected:
	Player& player;

	/// 入力方向へ向き＋移動する共通処理。
	bool MoveByInput(float speed, float dt);
};

/// 待機・移動・ダッシュ（地上の基本。ここから各アクションへ分岐）
class BaseMovement final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::BaseMovement); }

private:
	void ResetIdleTrigger();
	void PlayIdle();

	bool  wasDashMoving = false;   //前フレーム、ダッシュで移動していたか
	bool  runStopping	= false;   //runstop再生中か
	float runStopTimer	= 0.0f;    //runstopの経過時間
	bool  runToggled	= false;	//ランのトグル

	float idleTimer = 0.0f;
	float idleTrigger = 0.0f;
	bool  idleActing = false;
};

/// 回避（開始直後にジャスト回避受付ウィンドウ）
class DodgeState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	void OnExit() override;
	int  Id() const override { return static_cast<int>(PlayerStateId::Dodge); }
private:
	float  timer = 0.0f;
	VECTOR dir = {};
	bool   linkToRun = false;
};

/// ジャンプ（上昇・下降。空中で攻撃 → 落下攻撃）
class JumpState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::Jump); }

};

/// 落下攻撃（急降下→着地硬直）
class JumpAttackState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::JumpAttack); }
private:
	bool  landed = false;
	float recovery = 0.0f;
	bool  rising = false;    
	float riseTimer = 0.0f;
	float attackTimer = 0.0f;

	bool   homingActive = false;
	VECTOR homingDir = {};
};

/// 地上コンボ攻撃（COMBO_MAX 段。受付ウィンドウで派生）
class AttackState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void OnExit() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::Attack); }
private:
	float timer = 0.0f;
	bool  queued = false; 
	VECTOR CalcAttackDir();
	void   PlayAttackAnim();   
};

/// ジャスト回避カウンター突進
class CounterDashState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	void OnExit() override;
	int  Id() const override { return static_cast<int>(PlayerStateId::CounterDash); }
private:
	VECTOR targetCenter = {};   //ラッシュの中心位置（目標敵の位置）
	float  timer = 0.0f; //経過時間
	int    hitDoneCount = 0;    //既に入れたダメージの回数
	bool   hasTarget = false;

	void ApplyRushDamage();   //範囲内の敵にダメージを与える
};


/// 必殺技（前方突進。ゲージ消費）
class UltimateState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::Ultimate); }
private:
	float timer = 0.0f;
};

/// 被弾のけぞり
class DamagedState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	void SetKnockbackDir(const VECTOR& dir) { knockDir = dir; }
	int  Id() const override { return static_cast<int>(PlayerStateId::Damaged); }
private:
	float timer = 0.0f;
	VECTOR knockDir = VGet(0.0f, 0.0f, 0.0f);
};

class DeadState final : public PlayerState
{
public:
	using PlayerState::PlayerState;
	void OnEnter() override;
	void Update(float dt) override;
	int  Id() const override { return static_cast<int>(PlayerStateId::Dead); }

	bool IsAnimFinished() const { return animFinished; }   //死亡アニメが終わったか

private:
	bool animFinished = false;
};