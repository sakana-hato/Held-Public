#pragma once
#include "DxLib.h"
#include "Capsule.h"

/// <summary>
/// 弾（発射体）の所属。誰が撃ったか＝誰に当たるか
/// </summary>
enum class ProjectileOwner
{
	Player,   //プレイヤーの弾（敵に当たる）
	Enemy,    //敵・ボスの弾（プレイヤーに当たる）
};

/// <summary>
/// 弾（発射体）の基底クラス
/// プレイヤーの魔法もボスの魔法も、これを継承して作る。
/// まっすぐ飛ぶ弾が基本。追尾弾などは派生でUpdateを変える。
/// </summary>
class Projectile
{
public:
	virtual ~Projectile() = default;

	Projectile(const VECTOR& pos, const VECTOR& velocity, ProjectileOwner owner, float radius, float power)
		: pos(pos), velocity(velocity), owner(owner), radius(radius), power(power) 
	{
	}

	//毎フレーム更新。false を返したら消滅
	virtual bool Update(float dt)
	{
		pos.x += velocity.x * dt;
		pos.y += velocity.y * dt;
		pos.z += velocity.z * dt;

		//地面より下に行かない
		const float minY = groundY + radius;   //地面＋弾の半径
		if (pos.y < minY)
		{
			pos.y = minY;
			if (velocity.y < 0.0f) velocity.y = 0.0f;   //下向きの速度を止める
		}

		lifeTime += dt;
		return lifeTime < maxLife;
	}

	//描画（今は球で仮置き。エフェクトは後で）
	virtual void Draw() const
	{
		unsigned int col = (owner == ProjectileOwner::Enemy)? GetColor(255, 80, 200) : GetColor(80, 200, 255); 
		DrawSphere3D(pos, radius, 12, col, col, TRUE);
	}

	//当たり判定用の球
	VECTOR GetPos() const { return pos; }
	float  GetRadius() const { return radius; }
	float  GetPower() const { return power; }
	void SetGroundY(float y) { groundY = y; }
	ProjectileOwner GetOwner() const { return owner; }

	virtual void SetTarget(const VECTOR& t) {}

	bool IsAlive() const { return alive; }
	void Kill() { alive = false; }

	//跳ね返し：向きを反転し、所属をプレイヤーに変える
	void Reflect(const VECTOR& newDir, float speedMul = 1.2f)
	{
		const float speed = VSize(velocity) * speedMul;
		velocity = VScale(newDir, speed);
		owner = ProjectileOwner::Player;   //跳ね返したらプレイヤーの弾に
		reflected = true;
		lifeTime = 0.0f;   //寿命リセット
	}
	bool IsReflected() const { return reflected; }

	void SetEffectInstance(int inst) { effectInstance = inst; }
	int  GetEffectInstance() const { return effectInstance; }

protected:
	VECTOR pos;
	VECTOR velocity;
	ProjectileOwner owner;
	float  radius;
	float  power;

	bool   alive = true;
	bool   reflected = false;
	float  lifeTime = 0.0f;
	float  maxLife = 5.0f;   //寿命（秒）
	float groundY = 0.0f;

	int effectInstance = -1;   //追従するエフェクト
};