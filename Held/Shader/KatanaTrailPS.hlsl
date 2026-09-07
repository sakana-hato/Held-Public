// 刀トレイル（斬撃軌跡）用のピクセルシェーダー
// 頂点シェーダーから渡された色をベースに、質感を加える。

Texture2D TrailTexture      : register(t0);
SamplerState TrailSampler   : register(s0);

// C++ から渡すパラメータ（時間など）
cbuffer TrailParam : register(b4)
{
	float time;        // 経過時間（揺らめき用）
	float intensity;   // 発光の強さ（1.0=通常）
	float2 padding;    // 16バイト境界合わせ
    float4 trailColor; // 軌跡の色
};

struct PS_INPUT
{
	float4 Diffuse    : COLOR0;      // 頂点カラー（アルファ含む）
	float2 TexCoords0 : TEXCOORD0;   // テクスチャ座標
	float4 Position   : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float alpha = input.Diffuse.a;

    // テクスチャを読む（u=時間方向, v=幅方向）
    float4 tex = TrailTexture.Sample(TrailSampler, input.TexCoords0);

    // 色は定数バッファ、テクスチャの明るさを掛ける
    float4 color = float4(trailColor.rgb, alpha);
    color.rgb *= tex.rgb; // テクスチャの模様を反映

    float flicker = 0.85f + 0.15f * sin(time * 30.0f + input.TexCoords0.x * 10.0f);
    color.rgb *= intensity * flicker;

    //return float4(tex.rgb, alpha);
    return color;
}