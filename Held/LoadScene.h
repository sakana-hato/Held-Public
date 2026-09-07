#pragma once
#include "Scene.h"
#include "SharedContext.h"

/// <summary>
/// ロードシーン処理（非同期処理）
/// </summary>
class LoadScene final :public Scene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>共通データ
	explicit LoadScene(SharedContext& ctx, InputSystem& input)
		: Scene(input), ctx_(ctx){}

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LoadScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void OnEnter()			override;

	/// <summary>
	/// 終了
	/// </summary>
	void OnExit()			override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt)	override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()				override;

private:
	//共通データ
	SharedContext& ctx_;

	int  progressDummy = 0;   //進捗の仮表示用

	/// <summary>
	/// 非同期処理で処理する中身
	/// </summary>
	void StartAsyncLoad();

	/// <summary>
	/// 読み込み進捗度を描画する
	/// </summary>
	void DrawProgressBar() const;
};