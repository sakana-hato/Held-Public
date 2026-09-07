#include "DxLib.h"
#include "PostEffect.h"
#include "Config.h"

/// <summary>
/// シェーダーへ渡すパラメータ
/// </summary>
struct PostParam
{
	float monochromeRate;	//モノクロの強さ
	float glitchRate;		//グリッチの強さ
	float time;				//経過時間
	float damageRate;		//被弾赤フラッシュの強さ
};

PostEffect::~PostEffect()
{
	End();
}

void PostEffect::Init(int w,int h)
{
	End();

	_w = w;
	_h = h;

	sceneScreen = MakeScreen(w, h, FALSE); //3D用スクリーン
	
	psHandle = LoadPixelShader("Shader/PostEffects.pso");
	cbHandle = CreateShaderConstantBuffer(sizeof(PostParam));
}

void PostEffect::End()
{
	if (sceneScreen >= 0)
	{
		DeleteGraph(sceneScreen); 
		sceneScreen = -1;
	}

	if (cbHandle >= 0)
	{
		DeleteShaderConstantBuffer(cbHandle);
		cbHandle = -1; 
	}

	if (psHandle>= 0)
	{
		DeleteShader(psHandle); 
		psHandle = -1;
	}
}

void PostEffect::Update(float dt)
{
	time += dt;

	//波紋の進行
	if (rippleState == RippleState::Expanding)
	{
		rippleProgress += dt / Config::PostEffect::RIPPLE_DURATION;
		if (rippleProgress >= 1.0f)
		{
			rippleProgress = 1.0f;
			
		}
	}
	else if (rippleState == RippleState::Shrinking)
	{
		rippleProgress -= dt / Config::PostEffect::RIPPLE_SHRINK_DURATION;
		if (rippleProgress <= 0.0f)
		{
			rippleProgress = 0.0f;
			rippleState = RippleState::None;  
		}
	}

	//波紋の進行をシェーダーに渡す
	monochromeRate = (rippleState != RippleState::None) ? rippleProgress : 0.0f;
}

void PostEffect::BeginScene()const
{
	SetDrawScreen(sceneScreen);
	ClearDrawScreen();
}

void PostEffect::Composite()const
{
	SetDrawScreen(DX_SCREEN_BACK);

	if (psHandle >= 0)
	{
		// 定数バッファに今の値を書き込んでシェーダーへ渡す
		if (cbHandle >= 0)
		{
			PostParam* post = static_cast<PostParam*>(GetBufferShaderConstantBuffer(cbHandle));
			post->monochromeRate	= monochromeRate;
			post->glitchRate		= glitchRate;
			post->time				= time;
			post->damageRate		= damageRate;

			//書き込んだ内容をGPUへ転送
			UpdateShaderConstantBuffer(cbHandle);

			//b0のスロットに割り当てる
			SetShaderConstantBuffer(cbHandle, DX_SHADERTYPE_PIXEL, 0);
		}

		VERTEX2DSHADER v[6] = {};//画面全体を覆う四角形

		//四隅の頂点
		v[0].pos = VGet(0.0f, 0.0f, 0.0f);
		v[1].pos = VGet(static_cast<float>(_w), 0.0f, 0.0f);
		v[2].pos = VGet(0.0f, static_cast<float>(_h), 0.0f);
		v[3].pos = VGet(static_cast<float>(_w), static_cast<float>(_h), 0.0f);

		//テクスチャ座標
		v[0].u = 0.0f; 
		v[0].v = 0.0f;
		v[1].u = 1.0f;
		v[1].v = 0.0f;
		v[2].u = 0.0f;
		v[2].v = 1.0f;
		v[3].u = 1.0f;
		v[3].v = 1.0f;

		//su/svはDxLibが内部で使う補助のテクスチャ座標
		v[0].su = 0.0f;
		v[0].sv = 0.0f;
		v[1].su = 1.0f;
		v[1].sv = 0.0f;
		v[2].su = 0.0f;
		v[2].sv = 1.0f;
		v[3].su = 1.0f;
		v[3].sv = 1.0f;

		for (int i = 0; i < 4; ++i)
		{
			v[i].rhw = 1.0f;							// 2D描画
			v[i].dif = GetColorU8(255, 255, 255, 255);	// 頂点色
			v[i].spc = GetColorU8(0, 0, 0, 0);			// スペキュラは使わない
		}

		//4頂点から6頂点を作る
		v[4] = v[2];
		v[5] = v[1];

		//シェーダー描画で使用するテクスチャを設定する
		SetUseTextureToShader	(0, sceneScreen);

		//シェーダー描画に使用するピクセルシェーダーを設定する
		SetUsePixelShader		(psHandle);

		//2Dプリミティブ描画
		DrawPrimitive2DToShader	(v, 6, DX_PRIMTYPE_TRIANGLELIST);

		//シェーダー描画に使用するピクセルシェーダーを設定する
		SetUsePixelShader		(-1);

		//シェーダー描画で使用するテクスチャを設定する
		SetUseTextureToShader	(0, -1);
	}
	else
	{
		DrawGraph(0, 0, sceneScreen, FALSE);
	}
}

