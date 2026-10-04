#pragma once
#include "Config.h"
#include "Precompiled.h"

class Stage;

/// <summary>
/// 各キャラクターの基本データ
/// </summary>
struct CharacterData
{
	VECTOR pos				= VGet(0.0f, 0.0f, 0.0f);	//キャラクターワールド座標
	VECTOR velocity			= VGet(0.0f, 0.0f, 0.0f);	//水平速度
	float  facingYawDeg		= 0.0f;						//体も向き
	float vy				= 0.0f;						//垂直速度

	//カプセル
	float radius			= 0.0f;
	float height			= 0.0f;

	//キャラクターのHP
	float hp				= 0.0f;

	/// <summary>
	/// 死亡しているかどうかのヘルパー
	/// </summary>
	bool IsDead()const { return hp <= 0.0f; }

	/// <summary>
	/// 向いている方向のベクトル
	/// </summary>
	VECTOR Forward()const
	{
		const float r = facingYawDeg * DX_PI_F / 180.0f;
		return VGet(std::sin(r), 0.0f, std::cos(r));
	}

	//カプセルの二点
	VECTOR CapsuleBottom() const { return VGet(pos.x, pos.y + radius, pos.z); }
	VECTOR CapsuleTop()    const { return VGet(pos.x, pos.y + height - radius, pos.z); }
};

/// <summary>
/// 各キャラクターに継承する基底クラス
/// </summary>
class Character
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stage"></param>ステージ
	explicit Character(Stage& stage):stage(stage){}

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Character()						= default;

	//コピー禁止
	Character(const Character&)					= delete;
	Character& operator = (const Character&)	= delete;

	//共通データへのアクセス
	CharacterData& Data()				{ return data; }
	const CharacterData& Data() const	{ return data; }

	//位置・向きの共通ゲッター・セッター
	VECTOR	GetPosition()const				{ return data.pos; }
	void	SetPosition(const VECTOR& pos)	{ data.pos = pos; }
	VECTOR	GetForward()const				{ return data.Forward(); }

	//生きているか死んでいるか
	bool IsDead()  const { return data.IsDead(); }
	bool IsAlive() const { return !data.IsDead(); }

	//床の高さを返す
	float FloorYAt(const VECTOR& p) const;

	//重力を適用して地面に留める
	void  ApplyGravity(float dt);

	//旋回速度の範囲で少しずつ向き直る
	void  FaceTowardDeg(float targetYawDeg, float dt);

protected:

	virtual float Gravity()   const = 0;
	virtual float TurnSpeed() const = 0;

	CharacterData data;			//状態データ
	Stage& stage;				//ステージ
};