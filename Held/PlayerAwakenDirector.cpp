#include "DxLib.h"
#include "PlayerAwakenDirector.h"
#include "Config.h"


void PlayerAwakenDirector::Start()
{
    Begin();
    phase = AwakenPhase::FadeToBlack;
    timer = 0.0f;
    BuildShards();
}

void PlayerAwakenDirector::Update(float dt)
{
    if (!IsPlaying()) return;
    timer += dt;

    switch (phase)
    {
    case AwakenPhase::FadeToBlack:
        if (timer >= Config::Player::Awaken::FADE_TIME) { phase = AwakenPhase::ShowText1; timer = 0.0f; }
        break;
    case AwakenPhase::ShowText1:
        if (timer >= Config::Player::Awaken::TEXT1_TIME) { phase = AwakenPhase::ShowText2; timer = 0.0f; }
        break;
    case AwakenPhase::ShowText2:
        if (timer >= Config::Player::Awaken::TEXT2_TIME) { phase = AwakenPhase::Crack; timer = 0.0f; }
        break;
    case AwakenPhase::Crack:
        if (timer >= Config::Player::Awaken::CRACK_TIME) { phase = AwakenPhase::Shatter; timer = 0.0f; }
        break;
    case AwakenPhase::Shatter:
    {
        //破片を落下・回転させる
        const float gravity = 1200.0f;   //重力（下向きの加速）

        for (auto& s : shards)
        {
            s.vy += gravity * dt;      //重力で下向きに加速
            s.cx += s.vx * dt;         //横に移動
            s.cy += s.vy * dt;         //下に移動
            s.angle += s.angleSpeed * dt;   //回転
        }

        if (timer >= Config::Player::Awaken::SHATTER_TIME)
        {
            Finish();
        }
        break;
    }
    default: break;
    }
}

void PlayerAwakenDirector::Draw() const
{
    if (!IsPlaying()) return;

    const int w = Config::Window::WINDOW_W;
    const int h = Config::Window::WINDOW_H;

    if (phase == AwakenPhase::Shatter)
    {
        for (const auto& s : shards)
        {
            //4頂点を回転・移動して画面座標に
            float sx[4], sy[4];
            const float cosA = cosf(s.angle);
            const float sinA = sinf(s.angle);
            for (int i = 0; i < 4; ++i)
            {
                //回転
                float rx = s.px[i] * cosA - s.py[i] * sinA;
                float ry = s.px[i] * sinA + s.py[i] * cosA;
                sx[i] = s.cx + rx;
                sy[i] = s.cy + ry;
            }

            //4頂点の多角形を、三角形2つで塗る
            DrawTriangle(
                static_cast<int>(sx[0]), static_cast<int>(sy[0]),
                static_cast<int>(sx[1]), static_cast<int>(sy[1]),
                static_cast<int>(sx[2]), static_cast<int>(sy[2]),
                GetColor(0, 0, 0), TRUE);
            DrawTriangle(
                static_cast<int>(sx[0]), static_cast<int>(sy[0]),
                static_cast<int>(sx[2]), static_cast<int>(sy[2]),
                static_cast<int>(sx[3]), static_cast<int>(sy[3]),
                GetColor(0, 0, 0), TRUE);
        }
    }
    else
    {
        //暗転（フェーズに応じてalpha。FadeToBlackは徐々に、それ以降は真っ黒）
        int alpha = 255;
        if (phase == AwakenPhase::FadeToBlack)
        {
            alpha = static_cast<int>(255.0f * (timer / Config::Player::Awaken::FADE_TIME));
        }

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        DrawBox(0, 0, w, h, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        //テキスト（仮フォント、中央に）
        SetFontSize(48);
        if (phase == AwakenPhase::ShowText1 || phase == AwakenPhase::ShowText2)
        {
            const char* t1 = "私はまだ";
            const int tw1 = GetDrawStringWidth(t1, static_cast<int>(strlen(t1)));
            DrawString(w / 2 - tw1 / 2, h / 2 - 60, t1, GetColor(255, 255, 255));
        }
        if (phase == AwakenPhase::ShowText2)
        {
            const char* t2 = "負けたくない";
            const int tw2 = GetDrawStringWidth(t2, static_cast<int>(strlen(t2)));
            DrawString(w / 2 - tw2 / 2, h / 2 + 10, t2, GetColor(255, 255, 255));
        }
        SetFontSize(16);   //元に戻す

        if (phase == AwakenPhase::Crack)
        {
            DrawCracks();   //ヒビの線を描く
        }
    }
}

void PlayerAwakenDirector::DrawCracks() const
{
    const int w = Config::Window::WINDOW_W;
    const int h = Config::Window::WINDOW_H;
    const int cx = w / 2;
    const int cy = h / 2;

    //ヒビの進行（timer / CRACK_TIME で伸びる）
    const float progress = timer / Config::Player::Awaken::CRACK_TIME;

    //中央から放射状に何本かヒビを伸ばす
    const int lines = 8;
    for (int i = 0; i < lines; ++i)
    {
        const float a = DX_TWO_PI_F * i / lines + (i * 0.3f);
        const float len = 500.0f * progress;   //progress で伸びる
        const int ex = cx + static_cast<int>(cosf(a) * len);
        const int ey = cy + static_cast<int>(sinf(a) * len);
        DrawLine(cx, cy, ex, ey, GetColor(150, 150, 160), 2);
    }
}

void PlayerAwakenDirector::BuildShards()
{
    shards.clear();
    const int w = Config::Window::WINDOW_W;
    const int h = Config::Window::WINDOW_H;
    const int cols = 6;
    const int rows = 5;
    const float cw = static_cast<float>(w) / cols;
    const float ch = static_cast<float>(h) / rows;
    const float jitter = 30.0f;   //頂点をずらす量

    //まず格子の頂点を作って、ランダムにずらす
    std::vector<std::vector<VECTOR>> grid(rows + 1, std::vector<VECTOR>(cols + 1));
    for (int r = 0; r <= rows; ++r)
    {
        for (int c = 0; c <= cols; ++c)
        {
            float x = c * cw;
            float y = r * ch;
            //端でない頂点はランダムにずらす
            if (c > 0 && c < cols) x += (GetRand(static_cast<int>(jitter * 2)) - jitter);
            if (r > 0 && r < rows) y += (GetRand(static_cast<int>(jitter * 2)) - jitter);
            grid[r][c] = VGet(x, y, 0);
        }
    }

    //各セルを、4頂点の破片にする
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            Shard s;
            //4隅（格子の頂点）
            VECTOR p0 = grid[r][c];
            VECTOR p1 = grid[r][c + 1];
            VECTOR p2 = grid[r + 1][c + 1];
            VECTOR p3 = grid[r + 1][c];

            //中心を計算
            s.cx = (p0.x + p1.x + p2.x + p3.x) / 4.0f;
            s.cy = (p0.y + p1.y + p2.y + p3.y) / 4.0f;

            //頂点を中心からの相対位置で保存
            s.px[0] = p0.x - s.cx; s.py[0] = p0.y - s.cy;
            s.px[1] = p1.x - s.cx; s.py[1] = p1.y - s.cy;
            s.px[2] = p2.x - s.cx; s.py[2] = p2.y - s.cy;
            s.px[3] = p3.x - s.cx; s.py[3] = p3.y - s.cy;

            //初速（中央から外へ＋下へ）
            const float screenCx = w * 0.5f;
            s.vx = (s.cx - screenCx) * 0.5f + (GetRand(200) - 100);
            s.vy = 100.0f + GetRand(300);
            s.angle = 0.0f;
            s.angleSpeed = (GetRand(200) - 100) * 0.02f;
            shards.push_back(s);
        }
    }
}