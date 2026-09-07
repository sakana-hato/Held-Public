#pragma once
#include "Director.h"
#include "Precompiled.h"

enum class AwakenPhase
{
   FadeToBlack, ShowText1, ShowText2, Crack, Shatter
};

struct Shard
{
    //破片の4頂点（中心からの相対位置）
    float px[4], py[4];   //各頂点の位置
    float cx, cy;         //破片の中心位置
    float vx, vy;         //落下速度
    float angle;          //回転
    float angleSpeed;
};

class PlayerAwakenDirector : public Director
{
public:
    void Start();

    void Update(float dt);

    void Draw() const;
private:

    void DrawCracks() const;

    void BuildShards();   //破片を生成

    AwakenPhase phase = AwakenPhase::FadeToBlack;

    float timer = 0.0f;

    std::vector<Shard> shards;
};