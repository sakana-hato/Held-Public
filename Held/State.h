#pragma once

/// <summary>
/// プレイヤーや敵　ボスのstate基底クラス
/// </summary>
class state
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~state() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void OnEnter() {}

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	virtual void Update(float dt) {}

	/// <summary>
	/// 終了
	/// </summary>
	virtual void OnExit() {}

	/// <summary>
	/// 判別子
	/// </summary>
	virtual int Id() const = 0;
};
