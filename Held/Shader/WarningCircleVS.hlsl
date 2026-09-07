// 警告円の頂点シェーダー
// 地面に置いた四角ポリゴンを、3D空間から画面座標に変換し、uvを渡す。
// 刀トレイルの頂点シェーダーと同じ構造。

#include "VertexShader.h"

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float4 Specular : COLOR1;
    float4 TexCoords0 : TEXCOORD0;
    float4 TexCoords1 : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 Diffuse : COLOR0;
    float2 TexCoords0 : TEXCOORD0;
    float4 Position : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

	// 頂点は既にワールド座標。LocalWorldMatrixは通さず、ビュー→射影のみ。
    float4 worldPos = float4(input.Position, 1.0f);

    float4 viewPos;
    viewPos.x = dot(worldPos, g_Base.ViewMatrix[0]);
    viewPos.y = dot(worldPos, g_Base.ViewMatrix[1]);
    viewPos.z = dot(worldPos, g_Base.ViewMatrix[2]);
    viewPos.w = 1.0f;

    output.Position.x = dot(viewPos, g_Base.ProjectionMatrix[0]);
    output.Position.y = dot(viewPos, g_Base.ProjectionMatrix[1]);
    output.Position.z = dot(viewPos, g_Base.ProjectionMatrix[2]);
    output.Position.w = dot(viewPos, g_Base.ProjectionMatrix[3]);

    output.Diffuse = input.Diffuse;
    output.TexCoords0 = input.TexCoords0.xy;

    return output;
}
