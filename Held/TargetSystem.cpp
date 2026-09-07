#include "DxLib.h"
#include "TargetSystem.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Boss.h"

void TargetSystem::ToggleLock(EnemyManager& enemies, const VECTOR& playerPos)
{
	if (target != nullptr)
	{
		target = nullptr;
		return;
	}

	Enemy* nearest = enemies.FindNearest(playerPos, LOCK_RANGE);
	target = nearest;
}

void TargetSystem::SwitchTarget(EnemyManager& enemies, int dir)
{
	//ロックしていないときは切り替えしない
	if (target == nullptr)
	{
		return;
	}

	Enemy* next = enemies.NextAliveEnemy(target, dir);
	if (next != nullptr)
	{
		target = next;
	}
}

void TargetSystem::Update(EnemyManager& enemies)
{
	//ターゲットが死んだ/消えたら解除
	if (target != nullptr && target->IsDead())
	{
		//死んだら次の敵に自動で移す（いなければ解除）
		Enemy* next = enemies.NextAliveEnemy(target, +1);
		target = (next != target) ? next : nullptr;
	}
}

bool TargetSystem::GetTargetPosition(VECTOR& out) const
{
	if (boss)
	{
		out = boss->Comp().pos;
		return true;
	}
	if (target)
	{
		out = target->Comp().pos;
		return true;
	}
	return false;
}