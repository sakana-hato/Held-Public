#include "DxLib.h"
#include "ShadowMapReceiver.h"

void ShadowReceiver::SetPlane(const VECTOR& center, float half, float y)
{
    //四隅をXZ平面上に並べる
    p0 = VGet(center.x - half, y, center.z - half);
    p1 = VGet(center.x + half, y, center.z - half);
    p2 = VGet(center.x + half, y, center.z + half);
    p3 = VGet(center.x - half, y, center.z + half);
}

void ShadowReceiver::Draw() const
{
    //法線は真上で固定
    const VECTOR up = VGet(0.0f, 1.0f, 0.0f);

    //頂点を作る
    VERTEX3D v[6];
    const VECTOR pts[6] = { p0, p2, p1,  p0, p3, p2 }; //四隅から三角形2枚分の頂点を並べる

    for (int i = 0; i < 6; ++i)
    {
        v[i].pos = pts[i];
        v[i].norm = up;
        v[i].dif = GetColorU8(255, 255, 255, 255);
        v[i].spc = GetColorU8(0, 0, 0, 0);
        v[i].u = 0.0f; v[i].v = 0.0f;
        v[i].su = 0.0f; v[i].sv = 0.0f;
    }

    //影だけを見せたいので乗算合成
    SetUseLighting(TRUE);
    SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
    DrawPolygon3D(v, 2, DX_NONE_GRAPH, FALSE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}