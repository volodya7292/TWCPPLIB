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
		uint leaf_index = morton_code_indices[i - input.leaves_offset];
		uint node_index = gnb_offsets[leaf_index].node_offset;
		node.transform = gnb_offsets[leaf_index].transform;
		node.transform_inverse = gnb_offsets[leaf_index].transform_inverse;
		node.vertex_offset = gnb_offsets[leaf_index].vertex_offset;
		node.element_index = gnb_offsets[leaf_index].node_offset;
		node.bounds.pMin = gnb[node_index].bounds.pMin;
		node.bounds.pMax = gnb[node_index].bounds.pMax;

		Bounds bb = node.bounds;

		float3 vertices[8] = {
			float3(bb.pMin.x, bb.pMin.y, bb.pMin.z),
			float3(bb.pMin.x, bb.pMin.y, bb.pMax.z),
			float3(bb.pMin.x, bb.pMax.y, bb.pMin.z),
			float3(bb.pMin.x, bb.pMax.y, bb.pMax.z),
			float3(bb.pMax.x, bb.pMin.y, bb.pMin.z),
			float3(bb.pMax.x, bb.pMin.y, bb.pMax.z),
			float3(bb.pMax.x, bb.pMax.y, bb.pMin.z),
			float3(bb.pMax.x, bb.pMax.y, bb.pMax.z),
		};

		float3 npMin, npMax;
		for (uint d = 0; d < 8; d++) {
			vertices[d] = mul(node.transform, float4(vertices[d], 1)).xyz;
			if (d == 0) {
				npMin = npMax = vertices[d];
			} else {
				npMin = min(npMin, vertices[d]);
				npMax = max(npMax, vertices[d]);
			}
		}

		node.bounds.pMin = npMin;
		node.bounds.pMax = npMax;
	} else {
		node.element_index = -1;
		node.transform = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		node.transform_inverse = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		node.vertex_offset = -1;
		node.bounds.pMin = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		node.bounds.pMax = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	nodes[i] = node;
}