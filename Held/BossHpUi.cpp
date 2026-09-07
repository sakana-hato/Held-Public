#include "DxLib.h"
#include "Precompiled.h"
#include "BossHpUI.h"
#include "Config.h"

BossHpBarUI::BossHpBarUI()
{
	fontHandle = CreateFontToHandle(nullptr, Config::UI::BossHpBar::FONT_SIZE, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
	nameFontHandle = CreateFontToHandle(nullptr, Config::UI::BossHpBar::NAME_FONT_SIZE, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
}

BossHpBarUI::~BossHpBarUI()
{
	if (fontHandle >= 0)
	{
		DeleteFontToHandle(fontHandle);
	}
	if (nameFontHandle >= 0)
	{
		DeleteFontToHandle(nameFontHandle);
	}
}

void BossHpBarUI::OnHealthChanged(const HealthChangedEvent& e)
{
	targetRate = e.Rate();
	currentHp = e.currentHp;
	maxHp = e.maxHp;
}

void BossHpBarUI::StartRevealAnimation()
{
	visible = true;
	revealing = true;
	revealTimer = 0.0f;
	displayRate = 0.0f;   //空から始める
}

void BossHpBarUI::Update(float dt)
{
	if (!visible)
	{
		return;
	}

	if (revealing)
	{
		//みなぎる演出：空から目標割合へ、時間をかけて増やす
		revealTimer += dt;
		const float t = revealTimer / Config::UI::BossHpBar::REVEAL_TIME;

		if (t >= 1.0f)
		{
			//演出終了。以降は通常モード（実際のHPを表示）
			revealing = false;
			displayRate = targetRate;
		}
		else
		{
			//イーズアウトで、最後がゆっくり満ちる
			const float eased = 1.0f - (1.0f - t) * (1.0f - t);
			displayRate = targetRate * eased;
		}
	}
	else
	{
		//通常モード：目標割合へなめらかに追従（減るアニメ）
		const float diff = targetRate - displayRate;
		displayRate += diff * Config::UI::BossHpBar::FOLLOW_SPEED * dt;

		if (std::fabs(targetRate - displayRate) < 0.001f)
		{
			displayRate = targetRate;
		}
	}
}

void BossHpBarUI::Draw() const
{
	if (!visible)
	{
		return;
	}


	const int x = Config::UI::BossHpBar::BAR_X;
	const int y = Config::UI::BossHpBar::BAR_Y;
	const int w = Config::UI::BossHpBar::BAR_W;
	const int h = Config::UI::BossHpBar::BAR_H;

	

	//減った部分の下地
	DrawBox(x, y, x + w, y + h, GetColor(40, 20, 50), TRUE);

	//現在のHPバー
	const int barW = static_cast<int>(w * displayRate);
	if (barW > 0)
	{
		DrawBox(x, y, x + barW, y + h, GetColor(160, 60, 220), TRUE);

		//上側にハイライト
		DrawBox(x, y, x + barW, y + h / 3, GetColor(200, 120, 240), TRUE);
	}

	//枠線
	DrawBox(x, y, x + w, y + h, GetColor(220, 200, 230), FALSE);

	//ボス名（バーの左上）
	if (nameFontHandle >= 0)
	{
		const char* name = Config::UI::BossHpBar::BOSS_NAME;
		DrawStringToHandle(x + 1, y - Config::UI::BossHpBar::NAME_FONT_SIZE - 3, name, GetColor(0, 0, 0), nameFontHandle);
		DrawStringToHandle(x, y - Config::UI::BossHpBar::NAME_FONT_SIZE - 4, name, GetColor(230, 210, 240), nameFontHandle);
	}
}