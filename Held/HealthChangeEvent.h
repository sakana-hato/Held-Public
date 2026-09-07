#pragma once
/// <summary>
/// HPが変化したときに通知されるイベントデータ
/// </summary>
struct HealthChangedEvent
{
	float currentHp = 0.0f;   //現在HP
	float maxHp		= 0.0f;   //最大HP
	float delta		= 0.0f;   //変化量

	/// <summary>
	/// HPの割合（0.0から1.0）
	/// </summary>
	float Rate() const { return (maxHp > 0.0f) ? currentHp / maxHp : 0.0f; }
};