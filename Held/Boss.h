#pragma once
#include "Character.h"
#include "BossStateId.h"
#include "Capsule.h"
#include "Animator.h"
#include "BoneAttachment.h"
#include "WarningCircle.h"
#include "StunChicks.h"
#include "Precompiled.h"
#include "RockRing.h"
#include "HealthSubject.h"
#include "Difficulty.h"

//前方宣言
class  BossState;
class  BossAttack;
class  CameraSystem;
class  Stage;
class  Player;
class  ProjectileManager;

/// <summary>
/// ボスの本体 プレイヤーと敵と同じくstateマシン管理
/// コメントを書かなければ
/// </summary>
class Boss:public Character
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>共通データ
	explicit Boss(CameraSystem& camera, Stage& stage,Player& player, ProjectileManager& projectiles, Difficulty difficulty);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Boss();

	/// <summary>
	/// ボスのモデルをセットする
	/// </summary>
	/// <param name="handle">	</param>ハンドル
	/// <param name="scale">	</param>サイズ	
	void SetModel(int handle, float scale);

	/// <summary>
	/// ボスにつける大剣をセットする
	/// </summary>
	/// <param name="handle"></param>ハンドル
	void SetSword(int handle);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()const;

	/// <summary>
	/// 警告円を描画する
	/// </summary>
	void DrawWarning() const { warning.Draw(); }

	/// <summary>
	/// 警告線を描画する
	/// </summary>
	void DrawChargeWarning() const;

	/// <summary>
	/// 大剣を描画する
	/// </summary>
	void DrawSword();

	/// <summary>
	/// 大剣を手に移す
	/// </summary>
	void SheatheSword();  

	/// <summary>
	/// 大剣を描画するか
	/// </summary>
	/// <returns></returns>
	bool IsSwordDrawn() const { return swordDrawn; }

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="amount">		</param>量
	/// <param name="attackerPos">	</param>攻撃を受けた方法
	void TakeDamage(float amount, const VECTOR& attackerPos);

	/// <summary>
	/// ボスの行動stateを変更する
	/// </summary>
	/// <param name="id"></param>StateId
	void ChangeState(BossStateId id);

	/// <summary>
	/// 現在の使用しているstateId
	/// </summary>
	BossStateId CurrentStateId() const;

	/// <summary>
	/// ボスの体のカプセル
	/// </summary>
	Capsule GetBodyCapsule() const;

	/// <summary>
	/// ボスの攻撃用のカプセル
	/// </summary>
	Capsule GetAttackCapsule() const;   

	/// <summary>
	/// ボスの大剣用のカプセル
	/// </summary>
	/// <returns></returns>
	Capsule GetSwordBladeCapsule() const;

	bool IsBeamFiring() const;

	Capsule GetBeamCapsule() const;


	/// <summary>
	/// 攻撃が有効かどうかのセッター
	/// </summary>
	/// <param name="boo"></param>
	void    SetAttackActive(bool boo) { attackActive = boo; }

	/// <summary>
	/// 攻撃が有効かどうかのbool
	/// </summary>
	/// <returns></returns>
	bool    IsAttackActive() const { return attackActive; }

	/// <summary>
	/// 攻撃の判定を出す時間とボーン
	/// </summary>
	/// <param name="frame"></param>
	void    SetAttackFrame(int frame) { attackFrame = frame; }  

	//プレイヤー関連ヘルパー
	/// <summary>
	/// プレイヤーとの距離
	/// </summary>
	/// <returns></returns>
	float  DistanceToPlayer() const;

	VECTOR DirToPlayer() const;

	float  YawToPlayerDeg() const;

	void   MoveTowardPlayer(float speed, float dt);

	bool IsAttackJustDodgeWindow() const;

	void PlayAnimImmediate(int animModel, int animIndex, bool loop)
	{
		animator.PlayImmediate(animModel, animIndex, loop);
	}
	

	WarningCircle& Warning() { return warning; }

	//アニメ

	/// <summary>
	/// stateのアニメションを再生する
	/// </summary>
	/// <param name="animModel"></param>アニメションハンドル
	/// <param name="animIndex"></param>アニメション名
	/// <param name="loop">		</param>ループするか
	void PlayAnim(int animModel, int animIndex, bool loop);

	/// <summary>
	/// 叫びアニメションを再生
	/// </summary>
	void PlayRoar();

	/// <summary>
	/// ボスのintro処理が終了したか
	/// </summary>
	bool IsIntroFinished() const;

	/// <summary>
	/// アニメーターのアニメ再生
	/// </summary>
	Animator& Anim() { return animator; }

	//攻撃パターンの選択・実行

	/// <summary>
	/// 使用できる攻撃の選択する
	/// </summary>
	BossAttack* PickAttack();          

	/// <summary>
	/// 現在使用している攻撃
	/// </summary>
	BossAttack* CurrentAttack() { return currentAttack; }

	/// <summary>
	///　攻撃をセットする
	/// </summary>
	/// <param name="att"></param>
	void SetCurrentAttack(BossAttack* att) { currentAttack = att; }

	/// <summary>
	/// 攻撃のクールダウン
	/// </summary>
	/// <param name="dt"></param>
	void  TickCooldown(float dt) { if (attackCooldown > 0.0f) attackCooldown -= dt; }

	/// <summary>
	/// 攻撃ができるかどうか
	/// </summary>
	/// <returns></returns>
	bool  CanAttack() const { return attackCooldown <= 0.0f; }

	/// <summary>
	/// クールタイムを開始する
	/// </summary>
	void  StartCooldown();   

	void EnterStagger();

	void DrawBeamWarning() const;


	CameraSystem& Camera() { return camera; }
	Stage& GetStage() const { return stage; }
	ProjectileManager& GetProjectiles() { return projectiles; }

	int   ModelHandle() const { return modelHandle; }

	void UpdatePhase();

	bool IsActive() const { return active; }

	void Activate() { active = true; }

	bool HasHitPlayer() const { return hitPlayer; }

	void SetHitPlayer(bool b) { hitPlayer = b; }

	/// <summary>
	/// ボスの行動エリア範囲を返す関数
	/// </summary>
	bool IsPlayerInArena() const;

	bool IsCharging() const;

	RockRing& GetRockRing() { return rockRing; }

	void AddHealthObserver(HealthObserver* obs) { health.AddHealthObserver(obs); }

	void NotifyInitialHp() { health.NotifyHealthChanged(data.hp, Config::Boss::HP_MAX, 0.0f); }

	float HpRate() const { return (hpMax > 0.0f) ? (data.hp / hpMax) : 0.0f; }

	Player& GetPlayer() const { return player; }

#if defined(_DEBUG)
	void DebugAdjustSword();
	void DebugDrawSwordAdjust() const;
	void DebugDrawSwordAxis()const;
#endif

protected:
		float Gravity()   const override { return Config::Player::Status::GRAVITY; }
		float TurnSpeed() const override { return Config::Boss::TURN_SPEED; }

private:
	void BuildStates();
	void BuildAttacks();

	
	CameraSystem& camera;
	Player& player;
	ProjectileManager& projectiles;
	Difficulty difficulty;

	BoneAttachment	sword;
	WarningCircle	warning;
	StunChicks		stunChicks;
	RockRing		rockRing;
	HealthSubject	health;
	 
	Animator animator;
	int   modelHandle		= -1;
	float modelScale		= 1.0f;
	float modelYawOffsetDeg = 0.0f;

	bool hitPlayer			= false;

	float hpMax			= Config::Boss::HP_MAX;
	int   phase			= 0;
	bool  hitReceived	= false;

	//状態
	std::vector<std::unique_ptr<BossState>> states;
	BossState* current = nullptr;

	//攻撃パターン
	std::vector<std::unique_ptr<BossAttack>> attacks;
	BossAttack* currentAttack = nullptr;

	
	int swordBackFrame	= -1;		//背中のボーン
	int swordHandFrame	= -1;		//手のボーン
	bool swordDrawn		= false;	//手に持っているか

	//攻撃判定
	bool attackActive		= false;
	int  attackFrame		= -1;
	float attackCooldown	= 0.0f;

	bool active = false;

	std::vector<int> afterImageModels;   //残像用に複製したモデル
	static constexpr size_t AFTERIMAGE_MAX = 15;

#if defined(_DEBUG)
	bool  swordAdjustMode = false;
	VECTOR dbgSwordOffset = VGet(Config::BossSword::HAND_OFFSET_X, Config::BossSword::HAND_OFFSET_Y, Config::BossSword::HAND_OFFSET_Z);
	float dbgSwordPitch = Config::BossSword::HAND_PITCH_RAD;
	float dbgSwordYaw = Config::BossSword::HAND_YAW_RAD;
	float dbgSwordRoll = Config::BossSword::HAND_ROLL_RAD;
#endif
};