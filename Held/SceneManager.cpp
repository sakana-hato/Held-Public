#include "DxLib.h"
#include "SceneManager.h"
#include "Config.h"
#include "TitleScene.h"	
#include "LoadScene.h"	
#include "GameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"

void SceneManager::Init()
{
    //裏画面で描画する
    SetDrawScreen       (DX_SCREEN_BACK);

    //Zバッファを使用する
    SetUseZBuffer3D     (TRUE);

    //Zバッファに深度情報を書き込む
    SetWriteZBuffer3D   (TRUE);

    //裏面カリングを使用する
    SetUseBackCulling   (TRUE);

    //カメラの 手前クリップ距離と 奥クリップ距離を設定する
    SetCameraNearFar    (Config::Camera::CAM_NEAR, Config::Camera::CAM_FAR);

    //背景の色を指定する
    SetBackgroundColor  (20, 28, 38);

    //ライトを使用する
    SetUseLighting      (TRUE);

    //標準ライトを使用しない
    SetLightEnable      (FALSE);   

    //ディスプレイの垂直同期信号を持つ
    SetWaitVSyncFlag    (TRUE);

    //Xinputコントローラを使用する
    SetUseXInputFlag    (TRUE);

    //マウスカーソルを表示しない
    SetMouseDispFlag    (FALSE);

    //タイトルシーンへ行く
    ChangeScene(SceneId::Title);
}

void SceneManager::Run()
{
    //fps計測
    double prevTime     = GetNowHiPerformanceCount() / 1000000.0f;
    double accumulator  = 0.0f;

    while (ProcessMessage() == 0)
    {
        //ESCキーでタイトルへ（押しただけでタイトルに戻ってしまうので確認を後で追加）
        if (CheckHitKey(KEY_INPUT_ESCAPE))
        {
            break;
        }

        //経過時間計測
        const double now    = GetNowHiPerformanceCount() / 1000000.0;
        double frame        = now - prevTime;
        prevTime            = now;
        
        //スパイク対策
        if (frame > Config::FrameRate::FRAME_CLAMP)
        {
            frame = Config::FrameRate::FRAME_CLAMP;
        }

        accumulator += frame;

        int steps = 0;

        while (accumulator >= Config::FrameRate::FIXED_DT && steps < 5)
        {
            input.Update();
           
            if (current)
            {
                current->Update(static_cast<float>(Config::FrameRate::FIXED_DT));
                

                if (current->HasRequest())
                {
                    const SceneId next = current->Requested();
                    current->ClearRequest();

                    if (next == SceneId::Exit)
                    {
                        return; 
                    }

                    ChangeScene(next);
                }
            }
            accumulator -= Config::FrameRate::FIXED_DT;
            ++steps;
        }
        if (accumulator > Config::FrameRate::FIXED_DT)
        {
            accumulator = Config::FrameRate::FIXED_DT;
        }

        ClearDrawScreen();

        if (current)
        {
            current->Draw();
        }
        ScreenFlip();
    }
}


std::unique_ptr<Scene> SceneManager::CreateScene(SceneId id)
{
    switch (id)
    {
    case SceneId::Title:
        return std::make_unique<TitleScene> (ctx, input);

    case SceneId::Load:
        return std::make_unique<LoadScene>  (ctx, input);

    case SceneId::Game:
        return std::make_unique<GameScene>  (ctx, input);

    case SceneId::GameClear:
        return std::make_unique<GameClearScene>(ctx, input);

    case SceneId::GameOver:
        return std::make_unique<GameOverScene>(ctx, input);

    default:
        return nullptr;
    }
}

void SceneManager::ChangeScene(SceneId id)
{
    if (current)
    {
        current->OnExit();
    }

    current     = CreateScene(id);
    currentId   = id;

    if (current)
    {
        current->OnEnter();
    }
}