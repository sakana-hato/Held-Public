#pragma once
#include "PlayerEvent.h"

/// <summary>
/// プレイヤー戦闘イベントの観測者
/// </summary>
class PlayerEventObserver
{
public:
	virtual ~PlayerEventObserver() = default;

	/// <summary>
	/// 戦闘イベントが発生したときに呼ばれる
	/// </summary>
	/// <param name="ev"></param>発生したイベント
	virtual void OnPlayerEvent(PlayerEvent ev) = 0;
};