#pragma once
#include "Precompiled.h"
#include "Scene.h"    
#include "SharedContext.h"
#include "InputSystem.h"

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager()									= default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager()									= default;

	/// <summary>
	/// コピー禁止
	/// </summary>
	SceneManager(const SceneManager&)				= default;
	SceneManager& operator=(const SceneManager&)	= default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// ゲームのメインループ実行
	/// </summary>
	void Run();
private:
	std::unique_ptr<Scene>	current;					// 現在動いているシーンの保持
	SceneId					currentId = SceneId::Title;	// 現在シーンIdの記録 最初はタイトル
	SharedContext			ctx;						// 共有データ
	InputSystem input;

	/// <summary>
	/// 指定されたシーンIdからシーンを生成する
	/// </summary>
	/// <param name="id"></param>シーンId
	std::unique_ptr<Scene>CreateScene(SceneId id);

	/// <summary>
	/// シーンを切り替える
	/// </summary>
	/// <param name="id"></param>シーンId
	void ChangeScene                 (SceneId id);
};