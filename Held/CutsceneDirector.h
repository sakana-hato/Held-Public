#pragma once
#include "Director.h"

struct SharedContext;
class CameraSystem;
class InputSystem;
class Stage;
class Player;
class Boss;

/// <summary>
///  ボス部屋突入カットシーンの演出を管理する。
/// </summary>
class CutSceneDirector:public Director
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	explicit CutSceneDirector(SharedContext& ctx, CameraSystem& camera, InputSystem& input,Stage& stage,Player& player,Boss&boss)
		: ctx_(ctx), camera(camera), input(input),stage(stage),player(player),boss(boss) {}

	/// <summary>
	/// カットシーンを開始する
	/// </summary>
	void Start();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// スキップが終了したか
	/// </summary>
	void SkipToEnd();
private:
	/// カットシーンの進行段階
	enum class Phase
	{
		FadeOut,			//暗転
		DoorOpening,		//扉が開くのを待つ
		PlayerWalking,		//プレイヤーが自動で歩く
		DoorClosing,		//扉を閉じる（逆再生）
		BossRising,			//ボスが地面から出る
		BossRoar,			//ボスが叫ぶ
		SkipFadeOut,		//スキップ暗転
		SkipSwitch,			//スキップの状態切り替え
	};

	/// <summary>
	/// 次のフェーズへ移行する
	/// </summary>
	/// <param name="next"></param>移行先のフェーズ
	void EnterPhase(Phase next);

	SharedContext& ctx_;					// 共通データ参照
	CameraSystem& camera;
	InputSystem& input;
	Stage& stage;
	Player& player;
	Boss& boss;

	Phase phase				= Phase::FadeOut;	// 現在のフェーズ
	float timer				= 0.0f;			// 現在のフェーズが始まってからの経過時間
	bool walkAnimStarted	= false;		// 歩きアニメを再生済みか
	bool doorCloseStarted	= false;		// 扉を閉じる処理を開始済みかのフラグ
	bool skipRequested		= false;		// スキップが要求されたか
};