#pragma once

/// <summary>
/// 別モデルの指定ボーンに追従させて配置・描画する汎用クラス。
/// キャラのボーンに追従する付属物全般に使える。
/// 武器固有の振る舞いは持たない
/// </summary>
class BoneAttachment
{
public:
    void SetModel(int hand) { handle = hand; }
    void SetScale(float sca) { scale = sca; }
    void SetOffset(const MATRIX& mat) { offset = mat; }  

    void Update(int ownerModel, int followFrame);

    void Draw() const;

    int    GetModel() const { return handle; }
    MATRIX GetWorldMatrix() const { return world; }
    bool   IsValid() const { return handle >= 0; }

private:
    int    handle = -1;
    float  scale = 1.0f;
    MATRIX offset = MGetIdent();   //ボーンからのオフセット
    MATRIX world = MGetIdent();   //計算結果のワールド行列
};
