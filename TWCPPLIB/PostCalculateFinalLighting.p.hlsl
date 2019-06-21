#include "HLSLHelper.hlsli"

Texture2D<float4> g_diffuse : register(t0);
Texture2D<float4> g_emission : register(t1);

Texture2D<float4> rt_direct : register(t2);
Texture2D<float4> rt_direct_albedo : register(t3);
Texture2D<float4> rt_indirect : register(t4);
Texture2D<float4> rt_indirect_albedo : register(t5);

sampler sam : register(s0);

float4 main(VS_QUAD input) : SV_TARGET
{
	uint2 RT_SIZE, G_SIZE;
rt_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
g_diffuse.GetDimensions(G_SIZE.x, G_SIZE.y);

uint2 rt_pixel = input.tex_coord * RT_SIZE;
uint2 g_pixel = input.tex_coord * G_SIZE;


//return float4(1,0,0,1); //the red color

//float2 texcoord = float2(vI&1, vI>>1);
//float2 texcoord = float2(pos.x / 2 + 0.5f, -(pos.y) / 2 + 0.5f);

//return float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);

//return lerp(rt_result.Sample(sam, input.texCoord), g_albedo.Sample(sam, input.texCoord), 0.5);

return g_emission[g_pixel] + rt_direct[rt_pixel] * (g_diffuse[g_pixel]) + rt_indirect[rt_pixel] * (g_diffuse[g_pixel]);
}