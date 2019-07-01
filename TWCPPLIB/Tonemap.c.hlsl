#include "HLSLHelper.hlsli"

RWTexture2D<float4> g_texture;

float3 ToneMapACESFilmic(float3 x) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 G_SIZE;
	g_texture.GetDimensions(G_SIZE.x, G_SIZE.y);

	uint2 g_pixel = DTid.xy;
	if (any(g_pixel >= G_SIZE))
		return;

	float4 c_in = g_texture[g_pixel];
	float3 c_out = ToneMapACESFilmic(c_in.rgb);

	g_texture[g_pixel] = float4(c_out, c_in.a);
}