// 刀トレイル（斬撃軌跡）用の頂点シェーダー
// DxLib の DirectX11 頂点シェーダーのお作法に従う。
// VertexShader.h をインクルードすると、g_Base（座標変換行列）が使える。

#include "VertexShader.h"

// 頂点シェーダーの入力（DxLib の VERTEX3DSHADER に対応）
struct VS_INPUT
{
	float3 Position   : POSITION;    // ローカル座標
	float3 Normal     : NORMAL0;     // 法線
	float4 Diffuse    : COLOR0;      // 頂点カラー（アルファ含む）
	float4 Specular   : COLOR1;      // スペキュラ
	float4 TexCoords0 : TEXCOORD0;   // テクスチャ座標
	float4 TexCoords1 : TEXCOORD1;   // サブテクスチャ座標
};

// 頂点シェーダーの出力（ピクセルシェーダーへ渡す）
struct VS_OUTPUT
{
	float4 Diffuse    : COLOR0;      // 頂点カラー
	float2 TexCoords0 : TEXCOORD0;   // テクスチャ座標
	float4 Position   : SV_POSITION; // 画面（プロジェクション）座標
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// ローカル座標
	float4 localPos;
	localPos.xyz = input.Position;
	localPos.w   = 1.0f;

	// ローカル → ワールド
	float4 worldPos;
	worldPos.x = dot(localPos, g_Base.LocalWorldMatrix[0]);
	worldPos.y = dot(localPos, g_Base.LocalWorldMatrix[1]);
	worldPos.z = dot(localPos, g_Base.LocalWorldMatrix[2]);
	worldPos.w = 1.0f;

	// ワールド → ビュー
	float4 viewPos;
	viewPos.x = dot(worldPos, g_Base.ViewMatrix[0]);
	viewPos.y = dot(worldPos, g_Base.ViewMatrix[1]);
	viewPos.z = dot(worldPos, g_Base.ViewMatrix[2]);
	viewPos.w = 1.0f;

	// ビュー → プロジェクション（画面座標）
	output.Position.x = dot(viewPos, g_Base.ProjectionMatrix[0]);
	output.Position.y = dot(viewPos, g_Base.ProjectionMatrix[1]);
	output.Position.z = dot(viewPos, g_Base.ProjectionMatrix[2]);
	output.Position.w = dot(viewPos, g_Base.ProjectionMatrix[3]);

	// 頂点カラーとテクスチャ座標をそのまま渡す
	output.Diffuse    = input.Diffuse;
	output.TexCoords0 = input.TexCoords0.xy;

	return output;
}
