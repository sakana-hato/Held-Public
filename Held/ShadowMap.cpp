#include "DxLib.h"
#include "Shadowmap.h"

ShadowMap::~ShadowMap()
{
	End();
}

void ShadowMap::Init(int size )
{
	End();
	handle = MakeShadowMap(size, size);
}

void ShadowMap::End()
{
	if (handle >= 0)
	{
		DeleteShadowMap(handle);
		handle = -1;
	}
}

void ShadowMap::SetLightDirection(const VECTOR& dir)const
{
	if (handle >= 0)
	{
		SetShadowMapLightDirection(handle, dir);
	}
}

void ShadowMap::SetArea(const VECTOR& minPos, const VECTOR& maxPos)const
{
	if (handle >= 0)
	{
		SetShadowMapDrawArea(handle, minPos, maxPos);
	}
}

void ShadowMap::BeginCast()const
{
	if (handle >= 0)
	{
		ShadowMap_DrawSetup(handle);
	}
}

void ShadowMap::EndCast()const
{
	if (handle >= 0)
	{
		ShadowMap_DrawEnd();
	}
}

void ShadowMap::BeginReceive()const
{
	if (handle >= 0)
	{
		SetUseShadowMap(0, handle);
	}
}

void ShadowMap::EndReceive() const
{
	SetUseShadowMap(0, -1);
}
