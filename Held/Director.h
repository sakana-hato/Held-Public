#pragma once

/// <summary>
/// 各演出用のdirectorの基底クラス
/// カットシーンのようなカメラ等をジャックするものに使用する
/// </summary>
class Director
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual~Director() = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	virtual void Update(float dt) = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw()const {};

	/// <summary>
	/// 演出中かどうか
	/// </summary>
	bool IsPlaying()const { return active && !finished; }

	/// <summary>
	/// 演出が完了したか
	/// </summary>
	bool IsFinished() const { return finished; }

	/// <summary>
	/// 状態は初期化する
	/// </summary>
	void Reset() { active = false; finished = false; OnReset(); }
protected:
	/// <summary>
	/// 演出を開始状態にする
	/// </summary>
	void Begin() { active = true; finished = false; }

	/// <summary>
	/// 演出を完了状態にする
	/// </summary>
	void Finish() { finished = true; }

	/// <summary>
	/// 各director固有の初期化したいものがある場合に使用する
	/// </summary>
	virtual void OnReset() {}

private:
	bool active			= false;   //開始されたか
	bool finished		= false;   //完了したか
};