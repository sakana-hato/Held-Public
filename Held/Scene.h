#pragma once
#include "InputSystem.h"

//各シーンのid
enum class SceneId
{
	Title,		//タイトルシーン
	Load,		//ロードシーン
	Game,		//ゲームシーン
	GameClear,	//ゲームクリアシーン
	GameOver,	//ゲームオーバーシーン
	Exit,		//終了
};

/// <summary>
/// シーンの基底クラス 
/// </summary>
class Scene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="input"></param>操作関係
	explicit Scene(InputSystem& input) : input(input) {}

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Scene() = default;

	/// <summary>
	/// シーンの開始時に一度だけ呼ばれる初期化処理
	/// </summary>
	virtual void OnEnter(){}

	/// <summary>
	/// シーン終了時に一度だけ呼ばれる終了処理
	/// </summary>
	virtual void OnExit(){}

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	virtual void Update(float dt) = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// シーン切り替えの要求があったか
	/// </summary>
	bool HasRequest()const { return requested; }

	/// <summary>
	/// どのシーンに切り替えるか
	/// </summary>
	SceneId Requested() const { return requestId; }

	/// <summary>
	/// シーンの要求をリセットする
	/// </summary>
	void ClearRequest() { requested = false; }
protected:
	/// <summary>
	/// 次のシーンへ切り替える処理
	/// </summary>
	/// <param name="id"></param>シーンid
	void RequestChange(SceneId id)
	{
		requested = true;
		requestId = id;
	}

	InputSystem& input;
private:
	bool requested		= false;			//次のシーンに切り替えるかのフラグ
	SceneId requestId	= SceneId::Title;	//最初のシーンはタイトル
};
