#include "DxLib.h"
#include "Precompiled.h"
#include "CounterUi.h"
#include "InputSystem.h"
#include "Config.h"
#include "ResourceManager.h"

CounterUi::~CounterUi()
{
	if (_fontHandle >= 0)
	{
		DeleteFontToHandle(_fontHandle);
	}
}

void CounterUi::Init(int buttonImage, int keyImage,const InputSystem& input)
{
	_buttonImage	= buttonImage;
	_keyImage		= keyImage;
	_input			= &input;

	const std::string fontName	= ResourceManager::Instance().FontName(Config::UI::Counter::FONT_ID);
	const char* namePtr			= fontName.empty() ? nullptr : fontName.c_str();
	_fontHandle					= CreateFontToHandle(namePtr, Config::UI::Counter::FONT_SIZE, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	visible = false;   //ç≈èâÇÕîÒï\é¶
}

void CounterUi::OnPlayerEvent(PlayerEvent eve)
{
	switch (eve)
	{
	case PlayerEvent::JustDodgeSuccess:
		glowTimer = 0.0f;
		usePad = (_input != nullptr) ? _input->IsPadConnected() : false;
		visible = true;
		break;
	case PlayerEvent::CounterUsed:
		break;
	case PlayerEvent::SlowMoEnd:
		visible = false;
		break;
	default:
		break;
	}
}

void CounterUi::Update(float dt)
{
	if (!visible)
	{
		return;
	}

	glowTimer += dt;//î≠åıÇÃéûä‘ÇêiÇﬂÇÈ
}

void CounterUi::Draw()const
{
	if (!visible)
	{
		return;
	}

	const int image = usePad ? _buttonImage : _keyImage;
	if (image < 0)
	{
		return;
	}

	using namespace Config::UI::Counter;

	const float t		= glowTimer / GLOW_TIME;//î≠åıÇÃéûä‘
	const float clamped = (t > 1.0f) ? 1.0f : t;
	const int glowAdd	= static_cast<int>(GLOW_ADD * (1.0f - clamped));

	DrawRotaGraph3(POS_X, POS_Y, C_X, C_X, SCALE_X, SCALE_Y, ANGLE, image, TRUE);
	if (glowAdd > 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ADD, glowAdd);
		DrawRotaGraph3(POS_X, POS_Y, C_X, C_X, SCALE_X, SCALE_Y, ANGLE, image, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (_fontHandle >= 0)
	{
		const char* text = usePad ? Config::UI::Counter::TEXT_PAD : Config::UI::Counter::TEXT_KEY;
		const int textX = Config::UI::Counter::TEXT_X;  
		const int textY = Config::UI::Counter::TEXT_Y;

		DrawStringToHandle(textX + 1, textY + 1, text, GetColor(0, 0, 0), _fontHandle);
		DrawStringToHandle(textX, textY, text, GetColor(255, 255, 255), _fontHandle);

		if (glowAdd > 0)
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, glowAdd);
			DrawStringToHandle(textX, textY, text, GetColor(255, 255, 255), _fontHandle);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}