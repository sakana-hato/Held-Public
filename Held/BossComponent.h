#pragma once
#include "DxLib.h"
#include "Config.h"
#include "Precompiled.h"

/// <summary>
/// ボスの基本的なコンポーネント
/// </summary>
struct BossComponent
{
	//トランスフォーム
	VECTOR pos			= VGet(0.0f, 0.0f, 0.0f);   
	VECTOR velocity		= VGet(0.0f, 0.0f, 0.0f);
	float  facingYawDeg = 0.0f;
	float  vy			= 0.0f;

	//カプセル
	float radius = Config::Boss::RADIUS;
	float height = Config::Boss::HEIGHT;

	//ステータス
	float hp	= Config::Boss::HP_MAX;
	float hpMax = Config::Boss::HP_MAX;

	//フェーズ
	int phase = 0;

	//被弾管理
	bool hitReceived = false;

	//ヘルパー
	bool IsDead() const { return hp <= 0.0f; }

	//HP割合
	float HpRate() const { return (hpMax > 0.0f) ? (hp / hpMax) : 0.0f; }

	//ボスの正面
	VECTOR Forward() const
	{
		const float r = facingYawDeg * DX_PI_F / 180.0f;
		return VGet(std::sin(r), 0.0f, std::cos(r));
	}

	VECTOR CapsuleBottom() const { return VGet(pos.x, pos.y + radius, pos.z); }
	VECTOR CapsuleTop()    const { return VGet(pos.x, pos.y + height - radius, pos.z); }
};