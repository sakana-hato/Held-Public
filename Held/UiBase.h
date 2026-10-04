#pragma once

/// <summary>
/// 各UIが使用する基底クラス
/// 描画内容は各UIが持つ
/// </summary>
class UiBase
{
public:
	virtual ~UiBase() = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	virtual void Update(float dt) = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw()const = 0;

	/// <summary>
	/// 表示するかどうか
	/// </summary>
	bool IsVisible()const { return visible; }

	/// <summary>
	/// 表示か非表示かを切り替える
	/// </summary>
	/// <param name="boo"></param>bool
	void SetVisible(bool boo) { visible = boo; }

protected:
	bool visible = false;  //表示フラグ
};
