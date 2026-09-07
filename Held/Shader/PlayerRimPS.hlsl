// -------------------------------------------------------------------------------
// プレイヤーモデル用 リムライトピクセルシェーダー（DirectX11）
// DxLib 公式のテクスチャ・サンプラ名/レジスタに合わせた版。
// コンパイル: ShaderCompiler.exe /Tps_5_0 PlayerRimPS.hlsl
// -------------------------------------------------------------------------------

// DxLib 公式と同じ名前・レジスタにする（名前が違うとバインドされない可能性がある）
SamplerState g_DiffuseMapSampler : register( s0 );   // ディフューズマップサンプラ
Texture2D    g_DiffuseMapTexture : register( t0 );   // ディフューズマップテクスチャ

// C++ から渡すリムライトのパラメータ
cbuffer cbRimLight : register( b4 )
{
	float4 g_RimColor;     // rgb:リムの色  a:リムの強さ
	float4 g_RimParam;     // x:絞り(pow)  y:有効フラグ  z:ベース発光  w:予備
};

// 入力（頂点シェーダーの出力とセマンティクスを一致させる）
struct PS_INPUT
{
	float2 TexCoords0 : TEXCOORD0;   // テクスチャ座標
	float3 VPosition  : TEXCOORD1;   // ビュー空間の座標
	float3 VNormal    : TEXCOORD2;   // ビュー空間の法線
	float4 Position   : SV_POSITION;
};

struct PS_OUTPUT
{
	float4 Color0 : SV_TARGET;
};

PS_OUTPUT main( PS_INPUT PSInput )
{
	PS_OUTPUT PSOutput;

	// テクスチャの色
	float4 texColor = g_DiffuseMapTexture.Sample( g_DiffuseMapSampler, PSInput.TexCoords0 );

	// ベース色（テクスチャ）に、うっすら発光色を足す
    float3 baseColor = lerp(texColor.rgb, float3(0, 0, 0), g_RimParam.w) + g_RimColor.rgb * g_RimParam.z;

	// リムライト
	float3 normal  = normalize( PSInput.VNormal );
	float3 viewDir = normalize( -PSInput.VPosition );
	float  rim     = 1.0f - saturate( dot( normal, viewDir ) );
	rim = pow( rim, g_RimParam.x );
	float3 rimColor = g_RimColor.rgb * rim * g_RimColor.a;

	// 合成（有効フラグで、有効時はリム込み、無効時は素のテクスチャ）
	float3 color = ( baseColor + rimColor ) * g_RimParam.y + texColor.rgb * ( 1.0f - g_RimParam.y );

	PSOutput.Color0.rgb = color;
	PSOutput.Color0.a   = texColor.a;

	return PSOutput;
}
