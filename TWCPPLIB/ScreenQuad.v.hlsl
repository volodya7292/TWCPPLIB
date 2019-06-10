#include "HLSLHelper.hlsli"

VS_QUAD main(uint vI : SV_VERTEXID) {
	float2 texcoord = float2(vI&1, vI>>1);

	VS_QUAD output;
	output.tex_coord = texcoord;
	output.pos = float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);
	return output;
}