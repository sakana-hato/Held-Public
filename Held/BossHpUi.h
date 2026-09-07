#pragma once
#include "HealthObserver.h"

/// <summary>
/// ボスのHPバーUI（Observer）
/// HealthSubject を観察して、HPが変わったら表示を更新する。
/// 画面上部に、紫のHPバーとボス名を描画する。
/// 登場時は、HPが空から満タンへ「みなぎる」演出を再生する。
/// </summary>
class BossHpBarUI : public HealthObserver
{
public:
	BossHpBarUI();
	~BossHpBarUI();

	/// <summary>
	/// HP変化の通知を受け取る（Observer）
	/// </summary>
	void OnHealthChanged(const HealthChangedEvent& e) override;

	/// <summary>
	/// 表示を開始して、みなぎる演出を始める（カットシーン終了時に呼ぶ）
	/// </summary>
	void StartRevealAnimation();

	/// <summary>
	/// 毎フレームの更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 表示中かどうか
	/// </summary>
	bool IsVisible() const { return visible; }

private:
	bool  visible = false;   //表示中か（カットシーン終了で true）
	bool  revealing = false;   //みなぎる演出中か

	float targetRate = 1.0f;    //目標HP割合（通知で更新）
	float displayRate= 0.0f;    //表示中のHP割合
	float revealTimer = 0.0f;    //みなぎる演出の経過時間

	float currentHp = 0.0f;      //現在HP（数字表示用）
	float maxHp = 0.0f;      //最大HP

	int fontHandle = -1;     //数字用フォント
	int nameFontHandle = -1;     //名前用フォント
};