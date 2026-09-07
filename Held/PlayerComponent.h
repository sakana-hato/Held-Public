#pragma once
#include "DxLib.h"
#include "Config.h"
#include "Precompiled.h"

/// <summary>
/// プレイヤーの「状態データ」だけを持つコンポーネント。
/// </summary>
struct PlayerComponent
{
	//トランスフォーム 
	VECTOR pos = VGet(0.0f, Config::Graund::GROUND_Y, -900.0f);	//足元基準のワールド座標
	VECTOR velocity = VGet(0.0f, 0.0f, 0.0f);				//水平速度
	float  facingYawDeg = 180.0f;									//体の向き
	float  vy = 0.0f;									//垂直速度

	// カプセル
	float radius = Config::Player::Hit::PLAYER_RADIUS;
	float height = Config::Player::Hit::PLAYER_HEIGHT;

	//ステータス
	float hp = Config::Player::Status::PLAYER_HP_MAX;
	float ultGauge = 0.0f;
	bool  enhanced = false;	//HP20%以下の強化状態
	bool  dashOn = false;   //ダッシュのトグル状態
	//戦闘フラグ
	bool  invincible = false;	//回避無敵など
	int   comboIndex = 0;		//現在のコンボ段(0..COMBO_MAX-1)


	//ヘルパー
	bool  IsDead()  const { return hp <= 0.0f; }
	float HpRate()  const { return (Config::Player::Status::PLAYER_HP_MAX > 0.0f) ? hp / Config::Player::Status::PLAYER_HP_MAX : 0.0f; }

	//前方ベクトル
	VECTOR Forward() const
	{
		const float r = facingYawDeg * DX_PI_F / 180.0f;
		return VGet(std::sin(r), 0.0f, std::cos(r));
	}

	//カプセルの2端点
	VECTOR CapsuleBottom() const { return VGet(pos.x, pos.y + radius, pos.z); }
	VECTOR CapsuleTop()    const { return VGet(pos.x, pos.y + height - radius, pos.z); }
};