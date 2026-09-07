#pragma once

#include "Projectile.h"

//プレイヤーを緩やかに追尾する弾。
//完全には追わず、少しずつ向きを変えるので、避ける余地がある。
class HomingProjectile : public Projectile
{
public:
	HomingProjectile(const VECTOR& pos, const VECTOR& velocity, ProjectileOwner owner,
		float radius, float power, float turnRate)
		: Projectile(pos, velocity, owner, radius, power)
		, turnRate(turnRate) {
	}

	//毎フレーム、マネージャからプレイヤー位置を受け取る
	void SetTarget(const VECTOR& t) override { targetPos = t; hasTarget = true; }

	bool Update(float dt) override
	{
		if (hasTarget)
		{
			VECTOR toTarget = VSub(targetPos, pos);
			const float len = VSize(toTarget);
			if (len > 1e-4f)
			{
				VECTOR desiredDir = VScale(toTarget, 1.0f / len);
				const float speed = VSize(velocity);
				if (speed > 1e-4f)
				{
					VECTOR curDir = VScale(velocity, 1.0f / speed);
					VECTOR newDir = VAdd(curDir, VScale(desiredDir, turnRate * dt));
					const float nlen = VSize(newDir);
					if (nlen > 1e-4f)
					{
						newDir = VScale(newDir, 1.0f / nlen);
					}

					velocity = VScale(newDir, speed);
				}
			}
		}

		return Projectile::Update(dt);
	}

private:
	VECTOR targetPos = {};
	bool   hasTarget = false;
	float  turnRate;
};