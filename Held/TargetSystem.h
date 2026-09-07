#pragma once

class Enemy;
class EnemyManager;
class Boss;

/// <summary>
/// ターゲットロックの管理
/// </summary>
class TargetSystem
{
public:

	void ToggleLock(EnemyManager& enemies, const VECTOR& playerPos);

	void SwitchTarget(EnemyManager& enemies, int dir);

	void Update(EnemyManager& enemies);

	bool   IsLocked() const { return target != nullptr; }
	Enemy* GetTarget() const { return target; }
	void   Clear() { target = nullptr; }

	void LockBoss(Boss* b) { boss = b; target = nullptr; }
	void ClearBoss() { boss = nullptr; }
	Boss* GetBossTarget() const { return boss; }
	bool  IsBossLocked() const { return boss != nullptr; }

	//何かをロックしているか（雑魚orボス）
	bool IsAnyLocked() const { return target != nullptr || boss != nullptr; }

	bool GetTargetPosition(VECTOR& out) const;

private:
	Enemy* target = nullptr;

	Boss* boss = nullptr;

	//ロック可能な最大距離
	static constexpr float LOCK_RANGE = 2000.0f;
};
