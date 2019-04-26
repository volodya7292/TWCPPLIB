#include "HLSLHelper.hlsli"

struct InputData {
	uint node_offset;
};

RWStructuredBuffer<LBVHNode> glbvh_nodes : register(u0);
StructuredBuffer<LBVHNode> lbvh : register(t0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	glbvh_nodes[input.node_offset + DTid.x] = lbvh[DTid.x];
}