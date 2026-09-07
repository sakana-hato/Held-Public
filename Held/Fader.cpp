#include <DxLib.h>
#include "Fader.h"
#include "Config.h"

namespace
{
	constexpr unsigned int kColor = 0x000000;   // 黒
}

void Fader::Init()
{
	isFadeIn	= false;
	isFadeOut	= false;
	countFrame	= 0.0f;
	fadeSpeed	= 10.0f;
	_color		= kColor;
	isFadeNow	= false;
}

void Fader::Update()
{
	if (isFadeIn)
	{
		countFrame -= fadeSpeed;
		if (countFrame <= 0.0f)
		{
			countFrame	= 0.0f;
			isFadeIn	= false;
		}
	}

	if (isFadeOut)
	{
		countFrame += fadeSpeed;
		if (countFrame >= 255.0f)
		{
			countFrame	= 255.0f;
			isFadeOut	= false;
		}
	}

	isFadeNow = (isFadeIn || isFadeOut);
}


void Fader::Draw() const
{
	//完全に明るい状態なら描画しない
	if (countFrame <= 0.0f)
	{
		return;
	}

	//α値のセットする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(countFrame));
	DrawBox(0, 0, Config::Window::WINDOW_W, Config::Window::WINDOW_H, _color, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Fader::FadeIn(float speed)
{
	if (!isFadeIn)
	{
		countFrame	= 255.0f;   
		fadeSpeed	= speed;
		isFadeIn	= true;
		isFadeOut	= false;
	}
}

void Fader::FadeOut(float speed)
{
	if (!isFadeOut)
	{
		countFrame	= 0.0f;     
		fadeSpeed	= speed;
		isFadeOut	= true;
		isFadeIn	= false;
	}
}

bool Fader::IsFinishFadeIn() const
{
	return countFrame <= 0.0f && !isFadeOut;
}

bool Fader::IsFinishFadeOut() const
{
	return countFrame >= 255.0f && !isFadeIn;
}