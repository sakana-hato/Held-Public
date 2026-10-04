#pragma once
#include "Scene.h"
#include "TitleDirector.h"
#include "Stage.h"
#include "Player.h"
#include "PostEffect.h"
#include "CameraSystem.h"
#include "TargetSystem.h"
#include "Difficulty.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene final :public Scene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	explicit TitleScene(InputSystem& input, GameResult& result)
		: Scene(input,result) {}

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void OnEnter()			override;

	/// <summary>
	///　終了
	/// </summary>
	void OnExit()			override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt)	override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()				override;
private:

	float blinkTimer = 0.0f;//点滅カウンター
	bool fadingOut = false;//フェードアウト中か
	float logoTimer = 0.0f;   //ロゴが出てからの経過時間
	int fontHandle = -1;   //タイトル文字用フォント
	bool prevPressed = false;

	CameraSystem camera;
	Stage stage;
	std::unique_ptr<Player> player;
	std::unique_ptr<TitleDirector> titleDirector;
	TargetSystem target;                             
	PostEffect   post;
	Difficulty   difficulty = Difficulty::Normal;    
};