#include "HLSLHelper.hlsli"

Texture2D<float4> g_diffuse : register(t0);
Texture2D<float4> g_emission : register(t1);

Texture2D<float4> rt_direct : register(t2);
//Texture2D<float4> rt_direct_albedo : register(t3);
Texture2D<float4> rt_indirect : register(t4);
//Texture2D<float4> rt_indirect_albedo : register(t5);

sampler sam : register(s0);

float4 main(VS_QUAD input) : SV_TARGET
{
	uint2 RT_SIZE, G_SIZE;
    rt_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	g_diffuse.GetDimensions(G_SIZE.x, G_SIZE.y);

	uint2 rt_pixel = input.tex_coord * RT_SIZE;
	uint2 g_pixel = input.tex_coord * G_SIZE;


	float4 diffuse = max(5e-3f, g_diffuse[g_pixel] / PI);

	return g_emission[g_pixel] + (rt_direct[g_pixel] + rt_indirect[g_pixel]) * diffuse;
}