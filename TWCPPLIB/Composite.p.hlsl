#include "HLSLHelper.hlsli"

TextureCube<float4> diffuse;

sampler d2;

float4 main(VSQuadOutput input) : SV_TARGET {
	float4 d = diffuse.Sample(d2, float3(input.tex_coord, 0));
	return d;
}