#pragma once
#include "Scene.h"
#include "SharedContext.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene final :public Scene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>共通データ
	explicit TitleScene(SharedContext& ctx, InputSystem& input) 
		: Scene(input), ctx_(ctx) {}

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
	//void OnExit()			override;

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
	//共通データ
	SharedContext& ctx_;

	int blinkTimer = 0;//点滅カウンター(たぶん消す)
};