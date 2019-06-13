#include "HLSLHelper.hlsli"

Texture2D<float4> tex : register(t0);

float4 main(VS_QUAD input) : SV_TARGET
{
	uint2 SIZE;
    tex.GetDimensions(SIZE.x, SIZE.y);

	return tex[input.tex_coord * SIZE];
}