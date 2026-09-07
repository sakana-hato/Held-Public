// 警告円のピクセルシェーダー
// 地面に置いた四角ポリゴンに、円を描く。
// ・外周のリングは最初から見える（攻撃範囲を示す）
// ・内側が中心から外へ「満ちていく」（fillRate で制御）
// ・赤色で、少し脈動する

// C++ から渡すパラメータ
cbuffer WarningParam : register(b4)   // DxLib と競合しない b4
{
	float time;      // 経過時間（脈動用）
	float fillRate;  // 満ちる割合（0=空, 1=満ちきり）
	float2 padding;
	float4 circleColor;  // 円の色（赤系）
};

struct PS_INPUT
{
	float4 Diffuse    : COLOR0;
	float2 TexCoords0 : TEXCOORD0;
	float4 Position   : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	// uv(0〜1) の中心(0.5,0.5)からの距離（0〜約0.707）
	float2 uv = input.TexCoords0;
	float2 fromCenter = uv - float2(0.5f, 0.5f);
	float dist = length(fromCenter) * 2.0f;   // 0(中心)〜1(外周)に正規化

	// 円の外は描かない
	if (dist > 1.0f)
	{
		return float4(0, 0, 0, 0);
	}

	float alpha = 0.0f;
	float3 col = circleColor.rgb;

	// --- 外周リング（最初から見える攻撃範囲の輪郭）---
	const float ringInner = 0.88f;   // リングの内側
	const float ringOuter = 1.0f;    // リングの外側（円の縁）
	if (dist >= ringInner)
	{
		alpha = 1.0f;   // リングははっきり見せる
	}

	// --- 内側の満ちていく塗り（中心から fillRate の半径まで）---
	if (dist <= fillRate)
	{
		// 満ちた部分。中心ほど濃く、縁は少し薄く
		float innerAlpha = 0.4f + 0.3f * (1.0f - dist);
		alpha = max(alpha, innerAlpha);
	}

	// --- 満ちる最前線を明るく（満ちていく縁が光る）---
	float edge = abs(dist - fillRate);
	if (edge < 0.04f && fillRate > 0.01f && fillRate < 0.99f)
	{
		alpha = 1.0f;
		col = lerp(col, float3(1.0f, 1.0f, 0.8f), 0.6f);   // 最前線は明るく
	}

	// --- 脈動（時間で全体の明るさが揺れる）---
	float pulse = 0.8f + 0.2f * sin(time * 6.0f);
	col *= pulse;

	return float4(col, alpha * circleColor.a);
}
