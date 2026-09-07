#pragma once
#include "Precompiled.h"
#include "HealthObserver.h"

/// <summary>
/// HP‚ğ‚¿A•Ï‰»‚ğŠÏ@Ò‚É’Ê’m‚·‚é”íŠÏ@Ò
/// </summary>
class HealthSubject
{
public:
	/// <summary>
	/// ŠÏ@Ò‚ğ“o˜^‚·‚é
	/// </summary>
	void AddHealthObserver(HealthObserver* obs)
	{
		if (obs)
		{
			observers.push_back(obs);
		}
	}

	/// <summary>
	/// ŠÏ@Ò‚ğ‰ğœ‚·‚é
	/// </summary>
	void RemoveHealthObserver(HealthObserver* obs)
	{
		observers.erase(std::remove(observers.begin(), observers.end(), obs),observers.end());
	}

	/// <summary>
	/// HP•Ï‰»‚ğ‘SŠÏ@Ò‚É’Ê’m‚·‚é
	/// </summary>
	/// <param name="cur"></param>Œ»İHP
	/// <param name="max"></param>Å‘åHP
	/// <param name="delta"></param>•Ï‰»—Ê
	void NotifyHealthChanged(float cur, float max, float delta)
	{
		HealthChangedEvent e{ cur, max, delta };
		for (auto* obs : observers)
		{
			if (obs)
			{
				obs->OnHealthChanged(e);
			}
		}
	}

private:
	std::vector<HealthObserver*> observers;
};