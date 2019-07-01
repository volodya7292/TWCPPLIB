#include "HLSLHelper.hlsli"

RWTexture2D<float4> g_texture;

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 G_SIZE;
	g_texture.GetDimensions(G_SIZE.x, G_SIZE.y);

	uint2 g_pixel = DTid.xy;
	if (any(g_pixel >= G_SIZE))
		return;

	float4 c_in = g_texture[g_pixel];
	float4 c_out = c_in / (c_in + 0.25);

	g_texture[g_pixel] = c_out;
}