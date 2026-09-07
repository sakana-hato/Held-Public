#include "DxLib.h"
#include "TitleScene.h"
#include "Config.h"

void TitleScene::OnEnter()
{
	blinkTimer = 0;
}

void TitleScene::Update(float dt)
{
	blinkTimer++;
	if (blinkTimer >= Config::Title::TITLE_BLINK_FRAMES * 2)
	{
		blinkTimer = 0;
	}

	//特定のキーで次のシーンへ移行する
	if (CheckHitKey(KEY_INPUT_SPACE) || GetJoypadInputState(DX_INPUT_PAD1) & PAD_INPUT_A)
	{
		RequestChange(SceneId::Load);
		return;
	}
}

void TitleScene::Draw()
{
	DrawString(490, 300, "Action Game", GetColor(255, 255, 255));

	if (blinkTimer < Config::Title::TITLE_BLINK_FRAMES)
	{
		DrawString(390, 380, "Press SPACE or A Button", GetColor(200, 200, 200));
	}
}