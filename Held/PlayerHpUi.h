#pragma once
#include "DxLib.h"
#include "HealthObserver.h"

/// <summary>
/// プレイヤーのHPバーUI
/// </summary>
class PlayerHpUI : public HealthObserver
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerHpUI();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerHpUI();

	/// <summary>
	/// HP変化の通知を受け取る
	/// </summary>
	void OnHealthChanged(const HealthChangedEvent& event) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// HPを即座に減らす
	/// </summary>
	void SetEmptyImmediate() { targetRate = 0.0f; displayRate = 0.0f; currentHp = 0.0f; }

private:
	/// <summary>
	/// HP割合から色を作る
	/// </summary>
	/// <param name="rate"></param>割合
	unsigned int RateToColor(float rate) const;

	int fontHandle		= -1;		//フォント
	float targetRate	= 1.0f;		//目標のHP割合
	float displayRate	= 1.0f;		//表示中のHP割合
	float currentHp		= 0.0f;		//現在HP
	float maxHp			= 0.0f;		//最大HP
};