
// 頂点シェーダーから渡ってくる入力
struct PS_INPUT
{
    float4 Position     : SV_POSITION;  // 画面上の位置
    float4 Diffuse      : COLOR0;       // 頂点色
    float4 Specular     : COLOR1;       // スペキュラ色
    float2 TexCoords0   : TEXCOORD0;    // テクスチャ座標
    float2 TexCoords1   : TEXCOORD1;    // 補助のテクスチャ座標
};

cbuffer PostParam : register(b0)
{
    float monochromeRate;   // モノクロの強さ（0=通常、1=完全モノクロ）
    float glitchRate;       // グリッチの強さ（0=なし、1=最大）
    float time;             // 時間（ノイズを動かす用）
    float damageRate;       // 被弾赤フラッシュの強さ（0=なし、1=最大）
};

// 入力画像（sceneScreen）が入ってくるテクスチャとサンプラー
Texture2D DiffuseTexture    : register(t0);
SamplerState DiffuseSampler : register(s0);

// メイン関数
float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.TexCoords0; // 読み取り位置
    
    // 横方向グリッチ
    if (glitchRate > 0.0f)
    {
        // 横のライン単位でランダムにずらす
        float lineY     = floor(uv.y * 80.0f);                                      // 画面を30本の横ラインに分割
        float noise     = frac(sin(lineY * 12.9898f + time * 10.0f) * 43758.5453f); // 疑似乱数(01)
        float shift     = (noise - 0.5f) * 0.1f * glitchRate;                       // -0.050.05 × 強さ
        uv.x            += shift;                                                   // そのライン全体を横にずらす
    }
    float4 color = DiffuseTexture.Sample(DiffuseSampler, uv); // 加工後のUVで、入力画像から色を読む
    
     // 色ずれグリッチ
    if (glitchRate > 0.0f)
    {
        float off   = 0.01f * glitchRate;
        float r     = DiffuseTexture.Sample(DiffuseSampler, uv + float2(off, 0)).r;
        float b     = DiffuseTexture.Sample(DiffuseSampler, uv - float2(off, 0)).b;
        color.r     = r;
        color.b     = b;
    }
    
    
    // 入力画像から、この位置の色を読む
    color.rgb   *= 0.60f; // 全体を暗く
    color.r     *= 1.0f; // 赤を弱め
    color.b     *= 1.0f; // 青を強め
    
     //モノクロ
    if (monochromeRate > 0.0f)
    {
        // 画面中央からの距離
        float2 center           = float2(0.5f, 0.5f);
        float2 diff             = input.TexCoords0 - center;
        diff.x                  *= 1.777f;  // 横長画面の補正（16:9なら約1.78）
        float distFromCenter    = length(diff);
        float rippleRadius      = monochromeRate * 1.2f; // 波紋の半径

        // 境界を少しぼかす
        float edge          = 0.05f; // 境界のぼかし幅
        float monoAmount    = 1.0f - smoothstep(rippleRadius - edge, rippleRadius + edge, distFromCenter);

        // モノクロを適用
        float gray  = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
        color.rgb   = lerp(color.rgb, float3(gray, gray, gray), monoAmount);
    }
    
    // ビネット（縁を暗く）
    float2 center   = float2(0.7f, 0.7f);
    float dist      = distance(input.TexCoords0, center);
    float vig       = saturate(1.0f - dist * 0.5f);
    color.rgb       *= vig;
    
    // 被弾時に画面の外周を赤く染める
    if (damageRate > 0.0f)
    {
        float edge      = saturate(dist * 0.4f);        // 中心0 外側1
        float redAmount = edge * damageRate;            // 縁 × 被弾強さ
        color.rgb       = lerp(color.rgb, float3(0.8f, 0.0f, 0.0f), redAmount);
        
        float2 center   = float2(0.7f, 0.7f);
        float dist      = distance(input.TexCoords0, center);
        float vig       = saturate(1.0f - dist * 0.5f);
        color.rgb       *= vig;
    }
    return color;
}