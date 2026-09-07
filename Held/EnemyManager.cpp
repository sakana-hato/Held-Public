#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "EnemyManager.h"
#include "SharedContext.h"
#include "Stage.h"
#include "Player.h"

Enemy* EnemyManager::Spawn(int modelHandle, float scale, const VECTOR& pos)
{
	auto e = std::make_unique<Enemy>(ctx_,stage, player);
	e->SetModel(modelHandle, scale);
	e->SetPosition(pos);

	Enemy* raw = e.get();
	enemies.push_back(std::move(e));
	return raw;
}

void EnemyManager::Update(float dt, float deadDt)
{
	
	for (auto& e : enemies)
	{
		const float useDt = (e->CurrentStateId() == EnemyStateId::Dead) ? deadDt : dt;
		e->Update(useDt);
	}

	
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(),
			[](const std::unique_ptr<Enemy>& e) { return e->WantsRemove(); }),enemies.end());
}

void EnemyManager::Draw() const
{
	for (const auto& e : enemies)
	{
		e->Draw();
	}
}

void EnemyManager::DrawHpBars() const
{
	for (const auto& e : enemies)
	{
		if (!e->IsDead())
		{
			e->DrawHpBar();
		}
	}
}


bool EnemyManager::CheckPlayerAttack(const Capsule& blade, float power, int attackId, const VECTOR& attackerPos)
{
	if (blade.radius <= 0.0f)
	{
		return false;
	}

	if (attackId != lastAttackId_)
	{
		lastAttackId_ = attackId;
		hitThisAttack_.clear();
	}

	bool hitAny = false;   //1体でも当たったか

	for (auto& e : enemies)
	{
		if (e->IsDead()) continue;

		Enemy* raw = e.get();

		if (std::find(hitThisAttack_.begin(), hitThisAttack_.end(), raw) != hitThisAttack_.end())
		{
			continue;
		}

		if (CapsuleMath::Intersect(blade, e->GetBodyCapsule()))
		{
			e->TakeDamage(power, attackerPos);
			hitThisAttack_.push_back(raw);
			hitAny = true;   
		}
	}

	return hitAny;
}

bool EnemyManager::AnyEnemyEngaged() const
{
	for (const auto& e : enemies)
	{
		if (e->IsDead()) continue;
		const EnemyStateId s = e->CurrentStateId();
		
		if (s == EnemyStateId::Chase || s == EnemyStateId::Attack)
		{
			return true;
		}
	}
	return false;
}

void EnemyManager::ResolvePlayerCollision(Player& player)
{
	const VECTOR plpo = player.GetPosition();
	const float prc = player.GetBodyCapsule().radius;

	for (auto& ene : enemies)
	{
		if (ene->IsDead())
		{
			continue;
		}

		VECTOR enpo = ene->Comp().pos;
		const float erc = ene->Comp().radius;

		VECTOR di = VSub(enpo, plpo);
		di.y = 0.0f;
		const float dist = VSize(di);
		const float minDist = prc + erc + Config::Enemy::PUSH_MARGIN;

		if (dist < minDist && dist > 1e-4f)
		{
			const float push = minDist - dist;
			VECTOR dir = VScale(di, 1.0f / dist);   //プレイヤー→敵の方向

			//重さで分配：敵は少し、プレイヤーは残りを引く
			const float enemyMove = push * Config::Enemy::PUSH_RESISTANCE;        //敵が動く量
			const float playerMove = push * (1.0f - Config::Enemy::PUSH_RESISTANCE); //プレイヤーが押し返される量

			//敵を押す
			enpo.x += dir.x * enemyMove;
			enpo.z += dir.z * enemyMove;
			ene->Comp().pos = enpo;

			//プレイヤーを押し返す（敵→プレイヤー方向へ）
			VECTOR ppos = player.GetPosition();
			ppos.x -= dir.x * playerMove;
			ppos.z -= dir.z * playerMove;
			//プレイヤーも壁で止める
			ppos = player.GetStage().ResolveWall(ppos, prc, player.Comp().height);
			player.SetPosition(ppos);
		}
		else if (dist <= 1e-4f)
		{
			ene->Comp().pos.x += minDist;
		}

	}
}

int EnemyManager::AliveEnemyList(std::vector<Enemy*>& out) const
{
	out.clear();
	for (const auto& e : enemies)
	{
		if (!e->IsDead())
		{
			out.push_back(e.get());
		}

	}
	return static_cast<int>(out.size());
}

Enemy* EnemyManager::NextAliveEnemy(Enemy* current, int dir) const
{
	std::vector<Enemy*> list;
	AliveEnemyList(list);

	if (list.empty())
	{
		return nullptr;
	}


	int idx = -1;
	for (int i = 0; i < static_cast<int>(list.size()); ++i)
	{
		if (list[i] == current) 
		{ 
			idx = i;
			break;
		}
	}

	if (idx < 0)
	{
		return list[0];   //先頭
	}


	const int n = static_cast<int>(list.size());
	idx = (idx + dir + n) % n;
	return list[idx];
}

Enemy* EnemyManager::FindNearest(const VECTOR& from, float maxRange) const
{
	Enemy* best = nullptr;
	float bestSq = maxRange * maxRange;

	for (const auto& ene : enemies)
	{
		if (ene->IsDead())
		{
			continue;
		}

		VECTOR dir = VSub(ene->Comp().pos, from);
		dir.y = 0.0f;
		const float sq = dir.x * dir.x + dir.z * dir.z;
		if (sq < bestSq)
		{
			bestSq = sq;
			best = ene.get();
		}
	}
	return best;
}

bool EnemyManager::AnyAttackHitsSphere(const VECTOR& center, float radius) const
{
	for (const auto& ene : enemies)
	{
		if (ene->IsDead())
		{
			continue;
		}

		if (!ene->IsAttackActive())
		{
			continue;   
		}


		const Capsule atk = ene->GetAttackCapsule();
		Capsule sphere{ center, center, radius };
		if (CapsuleMath::Intersect(atk, sphere))
		{
			return true;
		}
	}
	return false;
}