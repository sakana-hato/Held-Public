#pragma once

/// <summary>
/// 地面の出す警告円
/// </summary>
class WarningCircle
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="center">  </param>円の中心座標（ワールド）
	/// <param name="radius">  </param>円の半径
	/// <param name="fillTime"></param>満ちきるまでの秒数
	void Init(const VECTOR& center, float radis, float fillTime);

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
	/// 表示中かどうか
	/// </summary>
	bool IsActive()const { return active; }

	/// <summary>
	/// 満ちきったかどうか
	/// </summary>
	bool IsFilled()const { return active && timer >= _fillTime; }

	/// <summary>
	/// 表示を終了する
	/// </summary>
	void Stop() { active = false; }

	/// <summary>
	/// 円の色を設定する
	/// </summary>
	/// <param name="r"></param>赤成分（0から1）
	/// <param name="g"></param>緑成分（0から1）
	/// <param name="b"></param>青成分（0から1）
	/// <param name="a"></param>不透明度（0から1）
	void SetColor(float r, float g, float b, float a) { _r = r; _g = g; _b = b; _a = a; }

private:
	
	VECTOR _center		= {};		// 円の中心座標
	float  _radius		= 100.0f;	// 円の半径
	float  _fillTime	= 1.0f;		// 満ちきるまでの秒数
	float  timer		= 0.0f;		// 表示開始からの経過時間
	bool   active		= false;	// 表示中かどうか
	
	float _r			= 1.0f, _g = 0.1f, _b = 0.1f, _a = 0.8f;// 色(赤)

};