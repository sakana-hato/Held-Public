#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "EnemyManager.h"
#include "SharedContext.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"

Enemy* EnemyManager::Spawn(int modelHandle, float scale, const VECTOR& pos)
{
	auto e = std::make_unique<Enemy>(stage, player);

	//モデルを複製して、敵ごとに独立したアニメーションにする
	const int dupModel = MV1DuplicateModel(modelHandle);
	e->SetModel(dupModel, scale);

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
			SoundManager::Instance().PlaySe(SeId::Attack_hit);
			SoundManager::Instance().PlaySe(SeId::EnemyHit);
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

		VECTOR enpo		= ene->Data().pos;
		const float erc = ene->Data().radius;

		VECTOR di				= VSub(enpo, plpo);
		di.y					= 0.0f;
		const float dist		= VSize(di);
		const float minDist		= prc + erc + Config::Enemy::PUSH_MARGIN;

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
			ene->Data().pos = enpo;

			//プレイヤーを押し返す（敵→プレイヤー方向へ）
			VECTOR ppos = player.GetPosition();
			ppos.x -= dir.x * playerMove;
			ppos.z -= dir.z * playerMove;
			//プレイヤーも壁で止める
			ppos = player.GetStage().ResolveWall(ppos, prc, player.Data().height);
			player.SetPosition(ppos);
		}
		else if (dist <= 1e-4f)
		{
			ene->Data().pos.x += minDist;
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

		VECTOR dir = VSub(ene->Data().pos, from);
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

void EnemyManager::ResolveEnemyCollision()
{
	//全ての敵の組み合わせを調べる
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		if (enemies[i]->IsDead())
		{
			continue;
		}

		for (size_t k = i + 1; k < enemies.size(); ++k)
		{
			if (enemies[k]->IsDead())
			{
				continue;
			}

			auto& a = enemies[i];
			auto& b = enemies[k];

			VECTOR posA = a->Data().pos;
			VECTOR posB = b->Data().pos;

			VECTOR diff = VSub(posB, posA);
			diff.y = 0.0f;
			const float dist = VSize(diff);

			const float minDist = a->Data().radius + b->Data().radius + Config::Enemy::PUSH_MARGIN;

			if (dist < minDist && dist > 1e-4f)
			{
				//重なっている分だけ、お互いを半分ずつ押し戻す
				const float push = (minDist - dist) * 0.5f;
				const VECTOR dir = VScale(diff, 1.0f / dist);   //AからBへの方向

				posA.x -= dir.x * push;
				posA.z -= dir.z * push;
				posB.x += dir.x * push;
				posB.z += dir.z * push;

				//壁にめり込まないように補正
				posA = stage.ResolveWall(posA, a->Data().radius, a->Data().height);
				posB = stage.ResolveWall(posB, b->Data().radius, b->Data().height);

				a->Data().pos = posA;
				b->Data().pos = posB;
			}
			else if (dist <= 1e-4f)
			{
				//完全に重なっている場合、片方を少しずらす
				b->Data().pos.x += minDist;
			}
		}
	}
}