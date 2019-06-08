#include "HLSLHelper.hlsli"

Texture2D<float4> g_albedo : register(t0);

Texture2D<float4> rt_direct : register(t1);
Texture2D<float4> rt_direct_albedo : register(t2);
Texture2D<float4> rt_indirect : register(t3);
Texture2D<float4> rt_indirect_albedo : register(t4);

sampler sam : register(s0);

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float2 tex_coord: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	uint2 SIZE;
    rt_direct.GetDimensions(SIZE.x, SIZE.y);

	uint2 pixel = input.tex_coord * SIZE;


	//return float4(1,0,0,1); //the red color

	//float2 texcoord = float2(vI&1, vI>>1);
	//float2 texcoord = float2(pos.x / 2 + 0.5f, -(pos.y) / 2 + 0.5f);

	//return float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);

	//return lerp(rt_result.Sample(sam, input.texCoord), g_albedo.Sample(sam, input.texCoord), 0.5);

	return rt_direct[pixel] * rt_direct_albedo[pixel] + rt_indirect[pixel] * rt_indirect_albedo[pixel];
}