#pragma once
#include "State.h"
#include "BossStateId.h"

class Boss;

/// <summary>
/// ボスの状態の中間基底 Boss への参照を持つ。
/// </summary>
class BossState : public state
{
public:
	explicit BossState(Boss& owner) : boss(owner) {}

protected:
	Boss& boss;
};
