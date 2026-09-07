#include "DxLib.h"
#include "GameClearScene.h"
#include "Config.h"
#include "InputAction.h"

void GameClearScene::OnEnter()
{
    rank        = CalcRank();
    cursorIndex = 0;
    inputLock   = true;
    lockTimer   = 0.0f;
}

void GameClearScene::OnExit()
{
   
}

void GameClearScene::Update(float dt)
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

void GameClearScene::Draw()
{
    DrawClear();
    DrawMenu();
}

char GameClearScene::CalcRank() const
{
    const float t = ctx_.clearTimeSec;

    if (t <= Config::Result::RANK_A_SEC) 
    {
        return 'A';
    }

    if (t <= Config::Result::RANK_B_SEC) 
    { 
        return 'B';
    }

    if (t <= Config::Result::RANK_C_SEC) 
    {
        return 'C'; 
    }

    if (t <= Config::Result::RANK_D_SEC) 
    { 
        return 'D'; 
    }
    return 'E';
}

void GameClearScene::DrawClear() const
{
    const int cx = Config::Window::WINDOW_W / 2; // 画面中央X

    SetFontSize(64);
    DrawString(cx - 180, 140, "GAME CLEAR", GetColor(255, 240, 120));
    SetFontSize(16);

    // 秒数を分と秒に分ける
    const int min = static_cast<int>(ctx_.clearTimeSec) / 60;
    const int sec = static_cast<int>(ctx_.clearTimeSec) % 60;

    char timeBuf[32];  // タイム表示用の文字列
    sprintf_s(timeBuf, sizeof(timeBuf), "Time : %02d:%02d", min, sec);
    DrawString(cx - 60, 240, timeBuf, GetColor(255, 255, 255));

    char rankBuf[16];  // ランク表示用の文字列
    sprintf_s(rankBuf, sizeof(rankBuf), "Rank : %c", rank);
    DrawString(cx - 60, 290, rankBuf, GetColor(255, 220, 50));
}

void GameClearScene::DrawMenu() const
{
    const int cx = Config::Window::WINDOW_W / 2;                // 画面中央X
    const unsigned int col_normal = GetColor(200, 200, 200);    // 未選択の色
    const unsigned int col_selected = GetColor(255, 255, 0);    // 選択中の色

    DrawString(cx - 60, 380, "Play Again",cursorIndex == 0 ? col_selected : col_normal);
    DrawString(cx - 60, 420, "Back to Title",cursorIndex == 1 ? col_selected : col_normal);
}