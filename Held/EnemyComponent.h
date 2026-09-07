#pragma once
#include "DxLib.h"
#include "Config.h"
#include "Precompiled.h"

/// <summary>
/// 雑魚敵のデータを持つコンポーネント
/// </summary>
struct EnemyComponent
{
	//トランスフォーム
	VECTOR pos				= VGet(0.0f, 0.0f, 0.0f); //足元基準のワールド座標
	VECTOR velocity			= VGet(0.0f, 0.0f, 0.0f); //水平速度
	float  facingYawDeg		= 0.0f;                   //体の向き
	float  vy				= 0.0f;                   //垂直速度

	//カプセル（当たり判定・仮表示）
	float radius = Config::Enemy::RADIUS;
	float height = Config::Enemy::HEIGHT;

	//ステータス
	float hp = Config::Enemy::HP_MAX;

	//被弾管理
	bool  hitReceived = false;   //このフレームに被弾したか

	//ヘルパー
	bool  IsDead() const { return hp <= 0.0f; }

	//向いている方向のベクトル
	VECTOR Forward() const
	{
		const float r = facingYawDeg * DX_PI_F / 180.0f;
		return VGet(std::sin(r), 0.0f, std::cos(r));
	}

	//カプセルの2端点
	VECTOR CapsuleBottom() const { return VGet(pos.x, pos.y + radius, pos.z); }
	VECTOR CapsuleTop()    const { return VGet(pos.x, pos.y + height - radius, pos.z); }
};