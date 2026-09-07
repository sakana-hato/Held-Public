#include "DxLib.h"
#include "Precompiled.h"
#include "WarningCircle.h"

void WarningCircle::Init(const VECTOR& center, float radius, float fillTime)
{
	active		= true;
	_center		= center;
	_radius		= radius;
	_fillTime	= (fillTime > 0.01f) ? fillTime : 0.01f;
	timer		= 0.0f;
}

void WarningCircle::Update(float dt)
{
	// 安全措置
	if (!active)
	{
		return;
	}

	timer += dt;
}

void WarningCircle::Draw() const
{
	// 安全措置
	if (!active)
	{
		return;
	}

	const float fillRate	= (timer / _fillTime > 1.0f) ? 1.0f : (timer / _fillTime);	// 満ち具合
	const int seg			= 48;														// 円の分割数
	const float y			= _center.y + 5.0f;											// 地面と重なってちらつかないよう少し浮かせる

	SetUseLighting		(FALSE);
	SetWriteZBuffer3D	(FALSE);
	SetDrawBlendMode	(DX_BLENDMODE_ALPHA, 180);
	SetUseBackCulling	(FALSE);
	
	const float innerR				= _radius * fillRate;	//満ちる内側の円半径
	const unsigned int fillColor	= GetColor(255, 40, 40);//色
	for (int i = 0; i < seg; ++i)
	{
		//扇形1枚分の角度
		const float a0 = DX_TWO_PI_F * i / seg;
		const float a1 = DX_TWO_PI_F * (i + 1) / seg;

		const VECTOR center = VGet(_center.x, y, _center.z);
		const VECTOR p0 = VGet(_center.x + std::cos(a0) * innerR, y, _center.z + std::sin(a0) * innerR);
		const VECTOR p1 = VGet(_center.x + std::cos(a1) * innerR, y, _center.z + std::sin(a1) * innerR);

		DrawTriangle3D(center, p0, p1, fillColor, TRUE);
	}
	SetDrawBlendMode	(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling	(TRUE);

	const unsigned int ringColor = GetColor(255, 80, 80);//外周リング
	for (int i = 0; i < seg; ++i)
	{
		const float a0	= DX_TWO_PI_F * i / seg;
		const float a1	= DX_TWO_PI_F * (i + 1) / seg;
		const VECTOR p0 = VGet(_center.x + std::cos(a0) * _radius, y, _center.z + std::sin(a0) * _radius);
		const VECTOR p1 = VGet(_center.x + std::cos(a1) * _radius, y, _center.z + std::sin(a1) * _radius);
		DrawLine3D(p0, p1, ringColor);
	}

	SetWriteZBuffer3D	(TRUE);
	SetUseLighting		(TRUE);
}