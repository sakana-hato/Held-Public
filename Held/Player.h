#pragma once
#include "Config.h"
#include "PlayerComponent.h"
#include "PlayerStateId.h"
#include "PlayerState.h"
//#include "State.h"
#include "BoneAttachment.h"
#include "Capsule.h"
#include "Katana.h"
#include "KatanaTrail.h"
#include "Precompiled.h"
#include "Animator.h"
#include "HealthSubject.h"
#include "Difficulty.h"


struct SharedContext;
class CameraSystem;
class InputSystem;
class Stage;
class EnemyManager;
class Boss;
class TargetSystem;

/// <summary>
/// プレイヤー本体。
/// 状態は PlayerStateで管理
/// データは PlayerComponent に集約
/// </summary>
class Player
{
public:
	explicit Player(SharedContext& ctx, CameraSystem& camera, InputSystem& input, Stage& stage,TargetSystem& target);
	~Player();

	Player(const Player&)				= delete;
	Player& operator=(const Player&)	= delete;

	/// <summary>
	/// プレイヤーのモデルをセットする
	/// </summary>
	/// <param name="handle">	</param>ハンドル
	/// <param name="scale">	</param>スケール
	void SetModel(int handle, float scale);

	/// <summary>
	/// 刀をセットする
	/// </summary>
	/// <param name="handle"></param>ハンドル
	void SetKatana(int handle);

	/// <summary>
	/// 鞘をセットする
	/// </summary>
	/// <param name="handle"></param>ハンドル
	void SetSheath(int handle);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	/// <param name="rawDt"></param>ジャスト回避演出用デルタタイム
	void Update(float dt, float rawDt);
	void Draw()const;

	float FloorYAt(const VECTOR& p) const;   // 足元の床の高さ

	VECTOR GetPosition()	 const { return comp.pos; }
	void	SetPosition(const VECTOR& po) { comp.pos = po; }
	VECTOR GetForward()		 const { return comp.Forward(); }
	Capsule GetBodyCapsule() const;
	float GetHp()			 const { return comp.hp; }
	float GetUltGauge()		 const { return comp.ultGauge; }
	bool  IsEnhanced()		 const { return comp.enhanced; }
	bool  IsInvincible()     const { return comp.invincible; }
	bool  IsDead()           const { return comp.IsDead(); }

	/// <summary>
	/// 被弾処理
	/// </summary>
	/// <param name="amount"></param>量
	void TakeDamage(float amount,const VECTOR& attackerPos);
	PlayerState* GetState(PlayerStateId id) { return states[static_cast<size_t>(id)].get(); }
	void AddUltGauge(float v);

	/// <summary>
	/// タイムスケール
	/// </summary>
	float GetWorldTimeScale() const;
	bool  IsSlowMoActive()    const { return slowMoTimer > 0.0f; }


	PlayerComponent& Comp() { return comp; }
	SharedContext& Ctx() { return ctx_; }

	InputSystem& Input() { return input; }
	Stage& GetStage() const { return stage; }

	void SetEnemies(EnemyManager* ene) { enemies = ene; }
	EnemyManager* GetEnemies() const { return enemies; }

	void SetBoss(Boss* b) { boss = b; }
	Boss* GetBoss() const { return boss; }

	TargetSystem& GetTarget() const { return target; }
	TargetSystem& GetTargetSystem() const { return target; }

	void ChangeState(PlayerStateId id);
	PlayerStateId CurrentStateId()const;

	// カメラの相対の方向に移動する
	VECTOR CalcMoveDirFromInput() const;

	void   FaceTowardDeg(float targetYawDeg, float dt);
	void   ApplyGravity(float dt);
	void   ApplyHorizontalMove(const VECTOR& dir, float speed, float dt);
	bool   IsGrounded() const { return comp.pos.y <= FloorYAt(comp.pos) + 0.01f; }

	//ジャスト回避まわりだよん
	void SetJustDodgeWindow(bool b) { justWindowActive = b; }
	bool IsJustDodgeWindow() const { return justWindowActive; }
	void TriggerJustDodgeSuccess();

	//アニメ再生
	void PlayAnim(int animModel, int animIndex, bool loop, bool useRootMotion);

	void PlayCutsceneWalk();

	//Animatorへの参照（State から再生したいとき用）
	Animator& Anim() { return animator; }

	/// <summary>
	/// 刀の処理関係
	/// </summary>
	void ToggleKatanaDraw();
	bool IsKatanaDrawn() const { return katana.IsDrawn(); }
	bool IsDrawingKatana() const { return drawingKatana || sheathingKatana; }

	void UpdateAnimOnly(float dt);
	void PlayCutsceneIdle();

	//攻撃判定
	void SetBladeActive(bool bla) { bladeActive = bla; }
	bool IsBladeActive() const { return bladeActive; }
	Capsule GetBladeCapsule() const { return katana.GetBladeCapsule(); }


	//回避判定
	void SetIncomingAttack(bool b) { incomingAttack = b; }
	bool IsIncomingAttack() const { return incomingAttack; }
	VECTOR GetJustDodgeSphereCenter() const { return VGet(comp.pos.x, comp.pos.y + comp.height * 0.5f, comp.pos.z); }
	float  GetJustDodgeSphereRadius() const { return Config::Player::Evasion::JUST_DODGE_SPHERE_RADIUS; }

	void TriggerSlowMo(float duration) { slowMoTimer = duration; }

	bool ShouldStartAwaken() const;   //覚醒演出を始めるべきか（HP条件＆まだ覚醒してない）
	void EnterEnhanced();             //覚醒演出の後、強化状態にする
	bool IsAwakenTriggered() const { return awakenTriggered; }

	int  GetAttackId() const { return attackIdCounter; }
	void NextAttackId() { attackIdCounter++; }   //攻撃を始めるたびに呼ぶ

	float GetCurrentAttackPower() const;

	float GetSlowMoTimer() const { return slowMoTimer; }

	float GetCurrentHitEffectScale() const;

	bool IsCounterDashUsed() const	{ return counterDashUsed_; }
	void MarkCounterDashUsed()		{ counterDashUsed_ = true; }

	void SetHidden(bool b) { hidden_ = b; }
	bool IsHidden() const { return hidden_; }

	void AddHealthObserver(HealthObserver* obs) { health.AddHealthObserver(obs); }
	void NotifyInitialHp() { health.NotifyHealthChanged(comp.hp, Config::Player::Status::PLAYER_HP_MAX, 0.0f); }

#if defined(_DEBUG)
	void DebugAdjustKatanaInput();      //Update用（値の更新）
	void DebugDrawKatanaAdjust() const; //Draw用（値の表示）
#endif
	
private:

	void BuildStates();           //状態インスタンス生成
	void UpdatePassive(float dt); //HP回復・強化判定・ゲージ自然増加・スロー減衰
	

	CameraSystem& camera;
	InputSystem& input;
	Stage& stage;
	EnemyManager* enemies = nullptr;
	Boss* boss = nullptr;
	TargetSystem& target;

	SharedContext& ctx_;
	PlayerComponent comp;
	Animator animator;
	Katana katana;
	KatanaTrail katanaTrail;
	HealthSubject health;   //HP変化を通知する被観察者
	Difficulty difficulty;

	BoneAttachment sheath;   
	int sheathHandle = -1;

	std::array<std::unique_ptr<PlayerState>, static_cast<size_t>(PlayerStateId::Count)> states;
	PlayerState* current = nullptr;

	int modelHandle			= -1;
	float modelScale		= 1.0f;
	float modelYawOffsetDeg = 0.0f;

	bool  justWindowActive	= false;	// ジャスト回避の受付中か
	float slowMoTimer		= 0.0f;		// 0 の間ワールドスロー

	bool  drawingKatana		= false;	//抜刀アニメ進行中か
	float drawTimer			= 0.0f;		//抜刀アニメの経過時間

	bool  sheathingKatana	= false;   //納刀アニメ進行中か
	float sheatheTimer		= 0.0f;

	bool bladeActive		= false;   //刀の攻撃判定が有効か
	bool incomingAttack = false;   //敵の攻撃が回避判定球に入っているか
	bool wasBladeActive = false;

	bool awakenTriggered = false;

	int  attackIdCounter = 0;

	bool counterDashUsed_ = false;   //このジャスト回避スロー中にカウンター突進を使ったか

	bool hidden_ = false;

	int rimVS_ = -1;   //リムライト頂点シェーダー
	int rimPS_ = -1;   //リムライトピクセルシェーダー
	int rimCB_ = -1;   //リムライトの定数バッファ

#if defined(_DEBUG)
	bool  katanaAdjustMode = false;
	VECTOR dbgKatanaOffset = VGet(Config::Katana::HAND_OFFSET_X, Config::Katana::HAND_OFFSET_Y, Config::Katana::HAND_OFFSET_Z);
	float dbgKatanaPitch = Config::Katana::HAND_PITCH_RAD;
	float dbgKatanaYaw = Config::Katana::HAND_YAW_RAD;
	float dbgKatanaRoll = Config::Katana::HAND_ROLL_RAD;
#endif
};