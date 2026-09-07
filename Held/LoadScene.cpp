#include "DxLib.h"
#include "LoadScene.h"
#include "Config.h"
#include "ResourceManager.h"

void LoadScene::OnEnter()
{
	progressDummy = 0;

	SetUseASyncLoadFlag(TRUE);//非同期処理を開始する
	StartAsyncLoad();
	SetUseASyncLoadFlag(FALSE);//非同期処理を終了する
}

void LoadScene::OnExit()
{
	//今は何もしないっぴ
}

void LoadScene::Update(float dt)
{
	//ロードが終了したらゲームシーンへ移行する
	if (GetASyncLoadNum() == 0)
	{
		RequestChange(SceneId::Game);
		return;
	}

	//仮
	if (progressDummy < 99)
	{
		progressDummy++;
	}

}

void LoadScene::Draw()
{
	DrawString(540, 320, "Now Loading...", GetColor(255, 255, 255));
	DrawProgressBar();
}

void LoadScene::StartAsyncLoad()
{
	ResourceManager::Instance().LoadJson("Data/json/Resources.json");
}

void LoadScene::DrawProgressBar() const
{
	constexpr int X = 340, Y = 360, W = 600, H = 20;

	// 枠
	DrawBox(X, Y, X + W, Y + H, GetColor(100, 100, 100), FALSE);

	// 進捗
	const int fill = W * progressDummy / 100;
	DrawBox(X, Y, X + fill, Y + H, GetColor(80, 180, 255), TRUE);
}