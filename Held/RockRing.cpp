#include "DxLib.h"
#include "RockRing.h"
#include "Config.h"

void RockRing::Init(int modelHandle)
{
	//安全措置
	if (modelHandle < 0)
	{
		return;
	}

	//本体モデルとは別に複製して、位置やスケールを自由に動かせるようにする
	handle = MV1DuplicateModel(modelHandle);
}

void RockRing::End()
{
	//モデルがあれば解放
	if (handle >= 0)
	{
		MV1DeleteModel(handle);
		handle = -1;
	}
}

void RockRing::Trigger(const VECTOR& center, float radius, float groundY)
{
	_center		= center;
	_radius		= radius;
	_groundY	= groundY;
	timer		= 0.0f;
	active		= true;
	phase		= Phase::Rising;
}

void RockRing::Update(float dt)
{
	if (!active)
	{
		return;
	}

	timer += dt;

	switch (phase)
	{
	case Phase::Rising:
		//せり上がりきったら維持へ
		if (timer >= Config::RockRing::RISE_TIME)
		{
			timer = 0.0f;
			phase = Phase::Hold;
		}
		break;

	case Phase::Hold:
		//維持し終わったら沈むへ
		if (timer >= Config::RockRing::HOLD_TIME)
		{
			timer = 0.0f;
			phase = Phase::Sinking;
		}
		break;

	case Phase::Sinking:
		//沈みきったら終了
		if (timer >= Config::RockRing::SINK_TIME)
		{
			active = false;
		}
		break;
	}
}

void RockRing::Draw() const
{
	if (!active || handle < 0)
	{
		return;
	}

	const float depth = Config::RockRing::RISE_DEPTH;   //地面下からの深さ

	//フェーズに応じて、現在の高さを決める
	float y = _groundY;
	switch (phase)
	{
	case Phase::Rising:
	{
		//地面の下から地面へ
		const float t = timer / Config::RockRing::RISE_TIME;
		const float eased = 1.0f - (1.0f - t) * (1.0f - t);   //イーズアウト
		y = _groundY - depth * (1.0f - eased);
		break;
	}
	case Phase::Hold:
		//地面の高さで静止
		y = _groundY;
		break;

	case Phase::Sinking:
	{
		//地面から下へ沈む
		const float t = timer / Config::RockRing::SINK_TIME;
		y = _groundY - depth * t;
		break;
	}
	}

	//攻撃範囲の半径に合わせてスケールを決める
	const float scale = _radius / Config::RockRing::MODEL_BASE_RADIUS;

	MV1SetScale		(handle, VGet(scale, scale, scale));
	MV1SetPosition	(handle, VGet(_center.x, y, _center.z));
	MV1DrawModel	(handle);
}