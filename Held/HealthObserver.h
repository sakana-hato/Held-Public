#pragma once
#include "HealthChangeEvent.h"

/// <summary>
/// HP変化を受け取る観察者インターフェース
/// </summary>
class HealthObserver
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HealthObserver() = default;

	/// <summary>
	/// HPが変化したときに呼ばれる
	/// </summary>
	/// <param name="e">変化の情報</param>
	virtual void OnHealthChanged(const HealthChangedEvent& e) = 0;
};
