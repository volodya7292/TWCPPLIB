#include "HLSLHelper.hlsli"

struct InputData {
	uint gnb_offsets_count;
	uint leaves_offset;
};

StructuredBuffer<LBVHNode> gnb : register(t0);
StructuredBuffer<SceneLBVHInstance> gnb_offsets : register(t1);
StructuredBuffer<uint4> morton_codes : register(t2);
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
		uint leaf_index = morton_codes[i - input.leaves_offset].y; // morton code index
		uint node_index = gnb_offsets[leaf_index].node_offset;
		node.instance = gnb_offsets[leaf_index];
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
			vertices[d] = mul(node.instance.transform, float4(vertices[d], 1)).xyz;
			if (d == 0) {
				npMin = npMax = vertices[d];
			} else {
				npMin = min(npMin, vertices[d]);
				npMax = max(npMax, vertices[d]);
			}
		}

		node.bounds.pMin = float4(npMin, 0);
		node.bounds.pMax = float4(npMax, 0);
	} else {
		node.instance.transform = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		node.instance.transform_inverse = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		node.instance.vertex_offset = -1;
		node.instance.node_offset = -1;
		node.bounds.pMin = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);
		node.bounds.pMax = float4(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	}

	nodes[i] = node;
}