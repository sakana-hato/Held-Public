#pragma once
#include "PlayerEventObserver.h"
#include "Precompiled.h"

// <summary>
/// プレイヤーの戦闘イベントを観察者に通知する被観察者
/// </summary>
class PlayerEventSubject
{
public:
	/// <summary>
	/// 観察者を登録する
	/// </summary>
	void AddObserver(PlayerEventObserver* obs)
	{
		if (obs)
		{
			observers.push_back(obs);
		}
	}

	/// <summary>
	/// 観察者を解除する
	/// </summary>
	void RemoveObserver(PlayerEventObserver* obs)
	{
		observers.erase(std::remove(observers.begin(), observers.end(), obs),observers.end());
	}

	/// <summary>
	/// イベントを全観察者に通知する
	/// </summary>
	/// <param name="ev"></param>発生したイベント
	void Notify(PlayerEvent ev)
	{
		for (auto* obs : observers)
		{
			if (obs)
			{
				obs->OnPlayerEvent(ev);
			}
		}
	}

private:
	std::vector<PlayerEventObserver*> observers;
};
