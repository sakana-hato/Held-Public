#pragma once

/// <summary>
/// カメラの基本的なコンポーネント
/// </summary>
struct CameraComponent
{
    float yaw       = 0.0f;     // 水平回転角
    float pitch     = 20.0f;    // 垂直回転角
    float distance  = 500.0f;   // 注視点からカメラまでの距離
    float fov       = 60.0f;    // 視野角

    //ピッチの上下制限
    static constexpr float PITCH_MIN =  -5.0f;  // カメラの最小ピッチ
    static constexpr float PITCH_MAX = 80.0f;   // カメラの最大ピッチ

    //距離の制限
    static constexpr float DISTANCE_MIN = 50.0f;    //カメラが近づける距離
    static constexpr float DISTANCE_MAX = 2000.0f;  //カメラが離れる最大距離

    /// <summary>
    /// 上下のカメラの角度を決める関数
    /// </summary>
    void ClampPitch()
    {
        if (pitch < PITCH_MIN)
        {
            pitch = PITCH_MIN;
        }

        if (pitch > PITCH_MAX)
        {
            pitch = PITCH_MAX;
        }
    }

    /// <summary>
    /// カメラをズームする際の最大距離と最小距離を決める関数(未使用）
    /// </summary>
    void ClampDistance()
    {
        if (distance < DISTANCE_MIN)
        {
            distance = DISTANCE_MIN;
        }

        if (distance > DISTANCE_MAX)
        {
            distance = DISTANCE_MAX;
        }
    }
};
