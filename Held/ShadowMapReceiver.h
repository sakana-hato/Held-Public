#pragma once
#include "Precompiled.h"

/// <summary>
/// 影を受けるためだけの地面平面(透明)
/// ステージにつけるとバグるため
/// </summary>
class ShadowReceiver
{
public:
    /// <summary>
    /// 地面をセットする
    /// </summary>
    /// <param name="center"></param>平面の中心座標
    /// <param name="half">  </param>中心から辺までの距離
    /// <param name="y">	 </param>平面を置く高さ
    void SetPlane(const VECTOR& center, float half, float y);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() const;
private:
    //四隅の座標
    VECTOR p0 = { -1000.0f, 0.0f, -1000.0f };
    VECTOR p1 = { 1000.0f, 0.0f, -1000.0f };
    VECTOR p2 = { 1000.0f, 0.0f,  1000.0f };
    VECTOR p3 = { -1000.0f, 0.0f,  1000.0f };
};
