#pragma once

//前方宣言
class Boss;

/// <summary>
/// ボスの攻撃パターンの基底クラス
/// </summary>
class BossAttack
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BossAttack() = default;

	/// <summary>
	/// 攻撃の開始
	/// </summary>
	virtual void OnStart(Boss& boss) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	virtual bool Update(Boss& boss, float dt) = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void OnEnd(Boss& boss) {}

	/// <summary>
	/// その攻撃を使える距離かどうか
	/// </summary>
	virtual bool IsUsable(const Boss& boss) const { return true; }

	/// <summary>
	/// 攻撃名
	/// </summary>
	virtual const char* Name() const { return "BossAttack"; }


	/// <summary>
	/// この攻撃が今、ジャスト回避を受け付けるタイミングか
	/// </summary>
	virtual bool IsJustDodgeWindow(const Boss& boss) const { return false; }
};
