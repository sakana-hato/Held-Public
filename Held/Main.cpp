#include "DxLib.h"
#include "SceneManager.h"
#include "Config.h"
#include "EffectManager.h"

int WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	//direct11を使用します
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	//ログファイルを出力します
	SetOutApplicationLogValidFlag(TRUE);

	//ウィンドウサイズを指定した大きさに適応します
	ChangeWindowMode(TRUE);

	//ウィンドウサイズを変更した際にグラフィックシステムをリセットしません
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	//ウィンドウサイズと色深度を指定します
	SetGraphMode(Config::Window::WINDOW_W, Config::Window::WINDOW_H, Config::Window::COLOR_DEPTH);

	//ゲーム名を表記します
	SetWindowText("Held");

	//初期化できなければ落とす
	if (DxLib_Init() == -1)
	{
		return -1;
	}

	//エフェクトマネージャーの初期化
	EffectManager::Instance().Init();

	SceneManager mgr;

	mgr.Init();
	mgr.Run();

	//エフェクトマネージャー終了
	EffectManager::Instance().End();
	DxLib_End();
	return 0;
}