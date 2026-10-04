#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "Character.h"
#include "Stage.h"
#include "Config.h"

float Character::FloorYAt(const VECTOR& p) const
{
	float y = Config::Graund::GROUND_Y;
	if (stage.GetFloorY(p, y))
	{
		return y;
	}
	return Config::Graund::GROUND_Y;
}

void Character::ApplyGravity(float dt)
{
	data.vy		-= Gravity() * dt;
	data.pos.y	+= data.vy * dt;

	const float floorY = FloorYAt(data.pos);
	if (data.pos.y <= floorY)
	{
		data.pos.y	= floorY;
		data.vy		= 0.0f;
	}
}

void Character::FaceTowardDeg(float targetYawDeg, float dt)
{
	float diff = targetYawDeg - data.facingYawDeg;
	while (diff > 180.0f)
	{
		diff -= 360.0f;
	}
	while (diff < -180.0f)
	{
		diff += 360.0f;
	}

	const float maxStep = TurnSpeed() * dt;
	if (std::fabs(diff) <= maxStep)
	{
		data.facingYawDeg = targetYawDeg;
	}
	else
	{
		data.facingYawDeg += (diff > 0.0f) ? maxStep : -maxStep;
	}

	while (data.facingYawDeg > 180.0f)
	{
		data.facingYawDeg -= 360.0f;
	}
	while (data.facingYawDeg < -180.0f)
	{
		data.facingYawDeg += 360.0f;
	}
}