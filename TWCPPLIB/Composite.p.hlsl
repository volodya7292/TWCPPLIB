#include "HLSLHelper.hlsli"

Texture2D<float4> diffuse;

sampler d2;

float4 main(VSQuadOutput input) : SV_TARGET {
	float4 d = diffuse.Sample(d2, input.tex_coord);
	return d;
}