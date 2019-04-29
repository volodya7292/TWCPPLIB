#include "HLSLHelper.hlsli"

struct InputData {
	uint gnb_offsets_count;
	uint leaves_offset;
};

StructuredBuffer<LBVHNode> gnb : register(t0);
StructuredBuffer<SceneLBVHInstance> gnb_offsets : register(t1);
StructuredBuffer<uint> morton_code_indices : register(t2);
RWStructuredBuffer<SceneLBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;

	SceneLBVHNode node;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	if (i >= input.leaves_offset) {
		uint node_index = gnb_offsets[i].offset;
		node.transform = gnb_offsets[i].transform;
		node.element_index = morton_code_indices[i - input.leaves_offset];
		node.bounds.pMin = gnb[node_index].bounds.pMin;
		node.bounds.pMax = gnb[node_index].bounds.pMax;

		float3 tpMin = mul(node.transform, float4(node.bounds.pMin, 1)).xyz;
		float3 tpMax = mul(node.transform, float4(node.bounds.pMax, 1)).xyz;
		float3 npMin = min(tpMin, min(tpMax, node.bounds.pMin));
		float3 npMax = max(tpMin, max(tpMax, node.bounds.pMax));
		node.bounds.pMin = npMin;
		node.bounds.pMax = npMax;
	} else {
		node.element_index = -1;
		node.transform = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		node.bounds.pMin = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		node.bounds.pMax = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	nodes[i] = node;
}