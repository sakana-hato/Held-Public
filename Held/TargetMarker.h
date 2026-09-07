#pragma once

/// <summary>
/// ターゲットで使用するモデルの読み込みと描画
/// </summary>
class TargetMarker
{
public:
    void Init(int modelHandle, float scale);
    void Update(float dt);
    void Draw(const VECTOR& targetHeadPos) const;   

private:
    int   _modelHandle = -1;
    float _scale = 1.0f;
    float _time = 0.0f;     
};