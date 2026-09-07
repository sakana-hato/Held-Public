#include "DxLib.h"
#include "Config.h"
#include "Precompiled.h"
#include "KatanaTrail.h"

struct TrailParam
{
	float time;
	float intensity;
	float padding[2];
	float trailColor[4];
};

void KatanaTrail::Init(int maxSegments)
{
	maxSegments = maxSegments;
	segments.clear();
	segments.reserve(maxSegments + 1);
}

void KatanaTrail::LoadShader()
{
	vsHandle = LoadVertexShader("Shader/KatanaTrailVS.vso");
	psHandle = LoadPixelShader("Shader/KatanaTrailPS.pso");
	cbHandle = CreateShaderConstantBuffer(sizeof(TrailParam));
	trailTexture = LoadGraph("Data/texture/slash.png");
}


void KatanaTrail::Clear()
{
	segments.clear();
}

void KatanaTrail::Update(float dt)
{
	time += dt;

	//発光を徐々に通常(1.0)へ戻す
	if (flashIntensity > 1.0f)
	{
		flashIntensity -= Config::Katana::TRAIL_FLASH_DECAY * dt;
		if (flashIntensity < 1.0f)
		{
			flashIntensity = 1.0f;
		}

	}
}


void KatanaTrail::Push(const VECTOR& root, const VECTOR& tip)
{
	segments.push_back({ root, tip });

	
	if (static_cast<int>(segments.size()) > maxSegments)
	{
		segments.erase(segments.begin());
	}
}

void KatanaTrail::Draw()const
{

	if (segments.size() < 2)
	{
		return;
	}

	if (vsHandle < 0 || psHandle < 0)
	{
		return;
	}


	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	//SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);

	if (cbHandle >= 0)
	{
		TrailParam* par = static_cast<TrailParam*>(GetBufferShaderConstantBuffer(cbHandle));
		par->time = time;
		par->intensity = flashIntensity;
		par->padding[0] = par->padding[1] = 0.0f;
		par->trailColor[0] = 0.63f;  // R (160/255)
		par->trailColor[1] = 0.82f;  // G (210/255)
		par->trailColor[2] = 1.0f;   // B (255/255)
		par->trailColor[3] = 1.0f;   // A
		UpdateShaderConstantBuffer(cbHandle);
		SetShaderConstantBuffer(cbHandle, DX_SHADERTYPE_PIXEL, 4);
	}

	//シェーダーを指定
	SetUseTextureToShader(0, trailTexture);
	SetUseVertexShader(vsHandle);
	SetUsePixelShader(psHandle);

	const int n = static_cast<int>(segments.size());
	const int R = 160, G = 210, B = 255;

	for (int i = 0; i < n - 1; ++i)
	{
		const Segment& a = segments[i];
		const Segment& b = segments[i + 1];

		const float alphaA = static_cast<float>(i) / (n - 1);
		const float alphaB = static_cast<float>(i + 1) / (n - 1);

		auto makeVert = [&](const VECTOR& pos, float alpha, float u, float v)
		{
				VERTEX3DSHADER vt = {};
				vt.pos = pos;
				vt.norm = VGet(0.0f, 1.0f, 0.0f);
				vt.dif = GetColorU8(R, G, B, static_cast<int>(alpha * 255.0f));
				vt.spc = GetColorU8(0, 0, 0, 0);
				vt.u = u; vt.v = v;
				vt.su = u; vt.sv = v;
				return vt;
		};

		VERTEX3DSHADER v[6];
		//u=時間方向(古い0 新しい1)、v=幅方向(根元0 切っ先1)
		v[0] = makeVert(a.root, alphaA, alphaA, 0.0f);
		v[1] = makeVert(a.tip, alphaA, alphaA, 1.0f);
		v[2] = makeVert(b.root, alphaB, alphaB, 0.0f);
		v[3] = makeVert(b.root, alphaB, alphaB, 0.0f);
		v[4] = makeVert(a.tip, alphaA, alphaA, 1.0f);
		v[5] = makeVert(b.tip, alphaB, alphaB, 1.0f);

		DrawPolygon3DToShader(v, 2);
	}

	SetUseVertexShader(-1);
	SetUsePixelShader(-1);
	SetWriteZBuffer3D(TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseTextureToShader(0, -1);
}