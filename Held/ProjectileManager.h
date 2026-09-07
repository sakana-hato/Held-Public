#include "DxLib.h"
#include "Projectile.h"
#include "Precompiled.h"
#include "EffectManager.h"

/// <summary>
/// 弾（発射体）を一括管理する。プレイヤーもボスもここに弾を発射する。
/// </summary>
class ProjectileManager
{
public:
	void Spawn(std::unique_ptr<Projectile> p)
	{
		projectiles.push_back(std::move(p));
	}

	//プレイヤー位置を受け取り、追尾弾に渡してから更新する
	void Update(float dt, const VECTOR& playerPos, const VECTOR& bossPos)
	{
		for (auto& p : projectiles)
		{
			if (!p->IsAlive())
			{
				continue;
			}

			if (p->GetOwner() == ProjectileOwner::Enemy)
			{
				p->SetTarget(playerPos);
			}
			else
			{
				p->SetTarget(bossPos);
			}
			

			if (!p->Update(dt))
			{
				p->Kill();
			}

			if (p->IsAlive() && p->GetEffectInstance() >= 0)
			{
				VECTOR effectPos = p->GetPos();
				EffectManager::Instance().SetPosition(p->GetEffectInstance(), effectPos);
			}
		}

		for (auto& p : projectiles)
		{
			if (!p->IsAlive() && p->GetEffectInstance() >= 0)
			{
				EffectManager::Instance().Stop(p->GetEffectInstance());
				p->SetEffectInstance(-1);
			}
		}

		//死んだ弾を除去
		projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),[](const std::unique_ptr<Projectile>& p) { return !p->IsAlive(); }),projectiles.end());
	}

	void Draw() const
	{
	}

	void Clear() { projectiles.clear(); }

	void SetDebugDraw(bool on) { debugDraw = on; }

	bool IsDebugDraw() const { return debugDraw; }

	std::vector<std::unique_ptr<Projectile>>& All() { return projectiles; }

private:
	std::vector<std::unique_ptr<Projectile>> projectiles;

	bool debugDraw = false;  
};