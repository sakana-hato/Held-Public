#include "DxLib.h"
#include "Precompiled.h"
#include "PlayerHpUI.h"
#include "Config.h"

PlayerHpUI::PlayerHpUI()
{
	//数字表示用のフォントを作成
	fontHandle = CreateFontToHandle(nullptr, Config::UI::PlayerHpBar::FONT_SIZE, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
}

PlayerHpUI::~PlayerHpUI()
{
	if (fontHandle >= 0)
	{
		DeleteFontToHandle(fontHandle);
	}
}

void PlayerHpUI::OnHealthChanged(const HealthChangedEvent& event)
{
	//通知を受けて、目標割合と数字を更新する
	targetRate	= event.Rate();
	currentHp	= event.currentHp;
	maxHp		= event.maxHp;
}

void PlayerHpUI::Update(float dt)
{
	//表示割合を目標割合へなめらかに追従させる
	const float diff = targetRate - displayRate;

	displayRate += diff * Config::UI::PlayerHpBar::FOLLOW_SPEED * dt;

	//ほぼ到達したら合わせる
	if (std::fabs(targetRate - displayRate) < 0.001f)
	{
		displayRate = targetRate;
	}
}

unsigned int PlayerHpUI::RateToColor(float rate) const
{
	
	int r, g, b;//色

	if (rate > 0.5f)
	{
		
		const float t	= (1.0f - rate) / 0.5f;  
		r				= static_cast<int>(0 + (230 - 0) * t);
		g				= 200;
		b				= 0;
	}
	else
	{
		
		const float t	= (0.5f - rate) / 0.5f;  
		r				= 230;
		g				= static_cast<int>(220 - 220 * t);
		b				= 0;
	}

	return GetColor(r, g, b);
}

void PlayerHpUI::Draw() const
{

	const int x = Config::UI::PlayerHpBar::BAR_X;
	const int y = Config::UI::PlayerHpBar::BAR_Y;
	const int w = Config::UI::PlayerHpBar::BAR_W;
	const int h = Config::UI::PlayerHpBar::BAR_H;

	//背景
	//DrawBox(x - FRAME_MARGIN, y - FRAME_MARGIN,x + w + FRAME_MARGIN, y + h + FRAME_MARGIN,GetColor(20, 20, 20), TRUE);

	//HPが減った部分
	DrawBox(x, y, x + w, y + h, GetColor(60, 20, 20), TRUE);

	//現在のHPバー
	const int barW = static_cast<int>(w * displayRate);
	if (barW > 0)
	{
		const unsigned int color = RateToColor(displayRate);
		DrawBox(x, y, x + barW, y + h, color, TRUE);
	}

	//枠線
	DrawBox(x, y, x + w, y + h, GetColor(230, 230, 230), FALSE);

	//数字
	if (fontHandle >= 0)
	{
		char buf[64];
		sprintf_s(buf, "%d / %d",static_cast<int>(currentHp + 0.5f),static_cast<int>(maxHp + 0.5f));

		const int textW = GetDrawStringWidthToHandle(buf, static_cast<int>(strlen(buf)), fontHandle);
		const int textX = x + (w - textW) / 2;
		const int textY = y + (h - Config::UI::PlayerHpBar::FONT_SIZE) / 2;

		//縁取り
		DrawStringToHandle(textX + 1, textY + 1, buf, GetColor(0, 0, 0), fontHandle);
		DrawStringToHandle(textX, textY, buf, GetColor(255, 255, 255), fontHandle);
	}
}