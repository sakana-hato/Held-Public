// -------------------------------------------------------------------------------
// プレイヤーモデル用 リムライト頂点シェーダー（DirectX11 / スキニングメッシュ 4ボーン）
//
// コンパイル: ShaderCompiler.exe /Tvs_5_0 PlayerRimVS.hlsl
// -------------------------------------------------------------------------------

#include "DataType.h"
#include "DxShader_VS_D3D11.h"

cbuffer cbD3D11_CONST_BUFFER_COMMON : register( b0 )
{
	DX_D3D11_CONST_BUFFER_COMMON		g_Common;
};

cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register( b1 )
{
	DX_D3D11_VS_CONST_BUFFER_BASE		g_Base;
};

cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX : register( b2 )
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX	g_OtherMatrix;
};

cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX : register( b3 )
{
	DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX	g_LocalWorldMatrix;
};

struct VS_INPUT
{
	float3 Position      : POSITION;
	float3 Normal        : NORMAL0;
	float4 Diffuse       : COLOR0;
	float4 Specular      : COLOR1;
	float4 TexCoords0    : TEXCOORD0;
	float4 TexCoords1    : TEXCOORD1;
	int4   BlendIndices0 : BLENDINDICES0;
	float4 BlendWeight0  : BLENDWEIGHT0;
};

struct VS_OUTPUT
{
	float2 TexCoords0 : TEXCOORD0;
	float3 VPosition  : TEXCOORD1;
	float3 VNormal    : TEXCOORD2;
	float4 Position   : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT VSInput )
{
	VS_OUTPUT VSOutput;

	int4   lBoneFloatIndex;
	float4 lLocalWorldMatrix[ 3 ];
	float4 lLocalPosition;
	float4 lWorldPosition;
	float4 lViewPosition;
	float3 lWorldNrm;
	float3 lViewNrm;

	lBoneFloatIndex = VSInput.BlendIndices0;

	lLocalWorldMatrix[ 0 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 0 ] * VSInput.BlendWeight0.xxxx;
	lLocalWorldMatrix[ 1 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 1 ] * VSInput.BlendWeight0.xxxx;
	lLocalWorldMatrix[ 2 ]  = g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.x + 2 ] * VSInput.BlendWeight0.xxxx;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 0 ] * VSInput.BlendWeight0.yyyy;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 1 ] * VSInput.BlendWeight0.yyyy;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.y + 2 ] * VSInput.BlendWeight0.yyyy;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 0 ] * VSInput.BlendWeight0.zzzz;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 1 ] * VSInput.BlendWeight0.zzzz;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.z + 2 ] * VSInput.BlendWeight0.zzzz;

	lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 0 ] * VSInput.BlendWeight0.wwww;
	lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 1 ] * VSInput.BlendWeight0.wwww;
	lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.Matrix[ lBoneFloatIndex.w + 2 ] * VSInput.BlendWeight0.wwww;

	lLocalPosition.xyz = VSInput.Position;
	lLocalPosition.w   = 1.0f;

	lWorldPosition.x = dot( lLocalPosition, lLocalWorldMatrix[ 0 ] );
	lWorldPosition.y = dot( lLocalPosition, lLocalWorldMatrix[ 1 ] );
	lWorldPosition.z = dot( lLocalPosition, lLocalWorldMatrix[ 2 ] );
	lWorldPosition.w = 1.0f;

	lViewPosition.x = dot( lWorldPosition, g_Base.ViewMatrix[ 0 ] );
	lViewPosition.y = dot( lWorldPosition, g_Base.ViewMatrix[ 1 ] );
	lViewPosition.z = dot( lWorldPosition, g_Base.ViewMatrix[ 2 ] );
	lViewPosition.w = 1.0f;

	VSOutput.Position.x = dot( lViewPosition, g_Base.ProjectionMatrix[ 0 ] );
	VSOutput.Position.y = dot( lViewPosition, g_Base.ProjectionMatrix[ 1 ] );
	VSOutput.Position.z = dot( lViewPosition, g_Base.ProjectionMatrix[ 2 ] );
	VSOutput.Position.w = dot( lViewPosition, g_Base.ProjectionMatrix[ 3 ] );

	lWorldNrm.x = dot( VSInput.Normal, lLocalWorldMatrix[ 0 ].xyz );
	lWorldNrm.y = dot( VSInput.Normal, lLocalWorldMatrix[ 1 ].xyz );
	lWorldNrm.z = dot( VSInput.Normal, lLocalWorldMatrix[ 2 ].xyz );

	lViewNrm.x = dot( lWorldNrm, g_Base.ViewMatrix[ 0 ].xyz );
	lViewNrm.y = dot( lWorldNrm, g_Base.ViewMatrix[ 1 ].xyz );
	lViewNrm.z = dot( lWorldNrm, g_Base.ViewMatrix[ 2 ].xyz );

	VSOutput.VPosition = lViewPosition.xyz;
	VSOutput.VNormal   = lViewNrm;

	VSOutput.TexCoords0.x = dot( VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[ 0 ][ 0 ] );
	VSOutput.TexCoords0.y = dot( VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[ 0 ][ 1 ] );

	return VSOutput;
}
