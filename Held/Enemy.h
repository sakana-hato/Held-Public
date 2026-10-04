#pragma once
#include "Character.h"
#include "Capsule.h"
#include "Animator.h"
#include "EnemyState.h"
#include "EnemyStateId.h"


class Stage;
class Player;

/// <summary>
/// 雑魚敵の本体クラス
 /// </summary>
class Enemy:public Character
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>共通データ
	explicit Enemy(Stage& stage,Player& player);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	//コピー禁止
	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;

	/// <summary>
	/// モデルを使用するための初期化など
	/// </summary>
	/// <param name="handle"></param>モデルハンドル
	/// <param name="scale"></param>サイズ
	void SetModel(int handle, float scale);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()const;

	/// <summary>
	/// //頭上のHPバー
	/// </summary>
	void DrawHpBar() const;   

	/// <summary>
	/// 当たり判定用カプセル
	/// </summary>
	Capsule GetBodyCapsule()const;

	/// <summary>
	/// 攻撃判定用のカプセル
	/// </summary>
	Capsule GetAttackCapsule() const;

	/// <summary>
	/// 攻撃判定の有効　無効を切り替える
	/// </summary>
	/// <param name="b"></param>bool
	void SetAttackActive(bool b) { attackActive = b; }

	/// <summary>
	/// 攻撃判定が有効か
	/// </summary>
	bool IsAttackActive() const { return attackActive; }

	/// <summary>
	/// 次に攻撃できるまでのクールダウン
	/// </summary>
	float GetAttackCooldown() const { return attackCooldown; }

	/// <summary>
	/// 攻撃クールダウンを設定する
	/// </summary>
	/// <param name="t"></param>時間
	void  SetAttackCooldown(float t) { attackCooldown = t; }

	/// <summary>
	/// 攻撃クールダウンを進める
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void  TickCooldown(float dt) { if (attackCooldown > 0.0f) attackCooldown -= dt; }

	/// <summary>
	/// 被弾処理
	/// </summary>
	/// <param name="amount"></param>量
	void TakeDamage(float amount, const VECTOR& attackerPos);

	/// <summary>
	/// 雑魚敵の状態を切り替える
	/// </summary>
	/// <param name="id"></param>敵の行動ID
	void ChangeState(EnemyStateId id);

	/// <summary>
	/// 状態は切り替える
	/// </summary>
	EnemyStateId CurrentStateId() const;


	/// <summary>
	/// 死亡して敵が消滅待ちか
	/// </summary>
	bool WantsRemove() const { return wantsRemove; }

	/// <summary>
	/// 雑魚敵の削除を要求する
	/// </summary>
	void SetWantsRemove() { wantsRemove = true; }

	/// <summary>
	/// プレイヤーへ向かって進む
	/// </summary>
	/// <param name="speed"></param>移動速度
	/// <param name="dt">   </param>デルタタイム。
	void  MoveTowardPlayer(float speed, float dt);

	/// <summary>
	/// プレイヤーまでの水平距離
	/// </summary>
	float DistanceToPlayer() const;

	/// <summary>
	/// プレイヤーへの向き
	/// </summary>
	VECTOR DirToPlayer() const;      

	/// <summary>
	/// プレイヤーがいる方向の角度
	/// </summary>
	float YawToPlayerDeg() const;

	/// <summary>
	/// アニメを再生する
	/// </summary>
	/// <param name="animModel"></param>アニメを持つモデル
	/// <param name="animIndex"></param>アニメ番号
	/// <param name="loop">		</param>ループ再生するか
	void PlayAnim(int animModel, int animIndex, bool loop);

	/// <summary>
	/// idle時のボイスタイマーをリセットする
	/// </summary>
	void ResetIdleVoiceTimer();

	/// <summary>
	/// 発見ボイスのハンドルを取得する
	/// </summary>
	int GetVoiceNoticeHandle() const { return voiceNoticeHandle; }

	/// <summary>
	/// 被弾ボイスのハンドルを取得する
	/// </summary>
	int GetVoiceDamagedHandle() const { return voiceDamagedHandle; }

	/// <summary>
	/// 攻撃ボイスのハンドルを取得する
	/// </summary>
	int GetVoiceAttackHandle() const { return voiceAttackHandle; }

	bool HasNoticedPlayer() const { return hasNoticedPlayer; }
	void SetNoticedPlayer(bool b) { hasNoticedPlayer = b; }

	/// <summary>
	/// アニメーターを取得する
	/// </summary>
	Animator& Anim() { return animator; }

	Stage& GetStage() const { return stage; }

	Player& GetPlayer() const { return player; }

protected:
		float Gravity()   const override { return Config::Player::Status::GRAVITY; }
		float TurnSpeed() const override { return Config::Enemy::TURN_SPEED; }
private:
	/// <summary>
	/// 各状態のインスタンスを生成する
	/// </summary>
	void BuildStates();

	/// <summary>
	/// 障害物を避けた進行方向を返す（未実装。現在は入力をそのまま返す）
	/// </summary>
	/// <param name="desiredDir"></param>本来進みたい方向
	/// <returns></returns>実際に進む方向
	VECTOR AvoidObstacles(const VECTOR& desiredDir) const;

	
	Player& player;
	Animator animator;		//アニメ再生

	//状態はIdをそのまま添字にして引く
	std::array<std::unique_ptr<EnemyState>, static_cast<size_t>(EnemyStateId::Count)>states;

	//現在の状態
	EnemyState* current = nullptr;

	int   modelHandle			= -1;		//モデルハンドル
	int   handFrame				= -1;		//攻撃判定を出す手ボーン
	float modelScale			= 1.0f;		//モデルサイズ
	float modelYawOffsetDeg		= 0.0f;		//モデルの向く方向
	float attackCooldown		= 0.0f;		//攻撃クールダウン
	bool  wantsRemove			= false;	//削除待ちかどうか
	bool  attackActive			= false;	//攻撃判定が有効か
	bool hasNoticedPlayer		= false;   //プレイヤーを発見済みか

	int voiceIdleHandle			= -1;   //待機ボイス
	int voiceNoticeHandle		= -1;   //発見ボイス
	int voiceDamagedHandle		= -1;   //被弾ボイス
	int voiceAttackHandle		= -1;   //攻撃ボイス

	float idleVoiceTimer		= 0.0f;   //待機ボイスのタイマー
	float idleVoiceInterval		= 0.0f;   //次に鳴らすまでの間隔（ランダム）
};