#include "HLSLHelper.hlsli"

struct InputData {
	uint gnb_offsets_count;
	uint leaves_offset;
};

StructuredBuffer<LBVHNode> gnb : register(t0);
StructuredBuffer<uint> gnb_offsets : register(t1);
StructuredBuffer<uint> morton_code_indices : register(t2);
RWStructuredBuffer<LBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;

	LBVHNode node;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	if (i >= input.leaves_offset) {
		uint node_index = gnb_offsets[i];
		node.element_index = morton_code_indices[i - input.leaves_offset];
		node.bounds.pMin = gnb[node_index].bounds.pMin;
		node.bounds.pMax = gnb[node_index].bounds.pMax;
	} else {
		node.element_index = -1;
		node.bounds.pMin = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		node.bounds.pMax = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	nodes[i] = node;
}