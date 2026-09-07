#include "DxLib.h"
#include "GameOverScene.h"
#include "Config.h"
#include "InputAction.h"

void GameOverScene::OnEnter()
{
    cursorIndex = 0;
    inputLock   = true;
    lockTimer   = 0.0f;
}

void GameOverScene::OnExit()
{
    
}

void GameOverScene::Update(float dt)
{
    // 即遷移防止
    if (inputLock)
    {
        lockTimer += dt;
        if (lockTimer >= INPUT_LOCK_SEC) 
        {
            inputLock = false; 
        
        }
        return;
    }

    // カーソル移動
    if (input.IsPressed(InputAction::MoveBack))
    {
        cursorIndex = (cursorIndex + 1) % 2;
    }

    if (input.IsPressed(InputAction::MoveForward))
    {
        cursorIndex = (cursorIndex + 1) % 2;
    }

    // 決定
    if (input.IsPressed(InputAction::Confirm))
    {
        RequestChange(cursorIndex == 0 ? SceneId::Load : SceneId::Title);
    }
}

void GameOverScene::Draw()
{
    // 背景は黒
    DrawBox(0, 0, Config::Window::WINDOW_W, Config::Window::WINDOW_H,GetColor(0, 0, 0), TRUE);

    DrawGameOver();
    DrawMenu();
}

void GameOverScene::DrawGameOver() const
{
    const int cx = Config::Window::WINDOW_W / 2;

    SetFontSize(72);
    DrawString(cx - 200, 150, "GAME OVER", GetColor(220, 40, 40));
    SetFontSize(16);
}

void GameOverScene::DrawMenu() const
{
    const int cx                    = Config::Window::WINDOW_W / 2; // 画面中央X
    const unsigned int col_normal   = GetColor(200, 200, 200);      // 未選択の色
    const unsigned int col_selected = GetColor(255, 255, 0);        // 選択中の色

    DrawString(cx - 60, 380, "Retry",cursorIndex == 0 ? col_selected : col_normal);
    DrawString(cx - 60, 420, "Back to Title",cursorIndex == 1 ? col_selected : col_normal);
}