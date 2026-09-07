#pragma once
#include "DxLib.h"
#include "Precompiled.h"

//強化状態のプレイヤーの周りに立ち上る赤いオーラ。
class PlayerAura
{
public:
	void Update(float dt, const VECTOR& playerPos)
	{
		//新しい粒を足元から発生
		spawnTimer_ -= dt;
		if (spawnTimer_ <= 0.0f)
		{
			spawnTimer_ = 0.02f;   //発生間隔
			Particle p;
			//プレイヤーの周りの円のどこかから
			const float a = (GetRand(628) / 100.0f);   //0〜2π
			const float r = 40.0f + GetRand(30);
			p.pos = VGet(playerPos.x + cosf(a) * r,playerPos.y + GetRand(20),  playerPos.z + sinf(a) * r);
			p.vy = 200.0f + GetRand(150);    //上へ立ち上る速度
			p.life = 0.0f;
			p.maxLife = 0.6f + GetRand(40) / 100.0f;
			p.size = 15.0f + GetRand(15);
			particles_.push_back(p);
		}

		//粒の更新
		for (auto& p : particles_)
		{
			p.pos.y += p.vy * dt;   //上へ
			p.life += dt;
		}
		//寿命切れを除去
		particles_.erase(
			std::remove_if(particles_.begin(), particles_.end(),
				[](const Particle& p) { return p.life >= p.maxLife; }),
			particles_.end());
	}

	void Draw() const
	{
		//加算合成で赤く光る粒
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		SetUseLighting(FALSE);
		for (const auto& p : particles_)
		{
			//寿命で薄くなる
			const float t = 1.0f - (p.life / p.maxLife);
			const int bright = static_cast<int>(255 * t);
			const unsigned int col = GetColor(bright, bright / 4, 0);   //赤〜オレンジ
			DrawSphere3D(p.pos, p.size * t, 8, col, col, TRUE);
		}
		SetUseLighting(TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	void Clear() { particles_.clear(); }

private:
	struct Particle
	{
		VECTOR pos;
		float vy;
		float life;
		float maxLife;
		float size;
	};
	std::vector<Particle> particles_;
	float spawnTimer_ = 0.0f;
};
