#include "HLSLHelper.hlsli"

struct InputData {
	uint gnb_offsets_count;
	uint leaves_offset;
	uint node_count;
};

StructuredBuffer<LBVHNode> gnb : register(t0);
StructuredBuffer<SceneLBVHInstance> gnb_offsets : register(t1);
StructuredBuffer<uint4> morton_codes : register(t2);
RWStructuredBuffer<SceneLBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	const uint i = DTid.x;

	if (i >= input.node_count)
		return;

	SceneLBVHNode node;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	if (i >= input.leaves_offset) {
		const uint leaf_index = morton_codes[i - input.leaves_offset].y; // morton code index
		const uint node_index = gnb_offsets[leaf_index].node_offset;
		node.instance = gnb_offsets[leaf_index];
		node.bounds.pMin = gnb[node_index].bounds.pMin;
		node.bounds.pMax = gnb[node_index].bounds.pMax;

		Bounds bb = node.bounds;

		const float3 vertices[8] = {
			float3(bb.pMin.x, bb.pMin.y, bb.pMin.z),
			float3(bb.pMin.x, bb.pMin.y, bb.pMax.z),
			float3(bb.pMin.x, bb.pMax.y, bb.pMin.z),
			float3(bb.pMin.x, bb.pMax.y, bb.pMax.z),
			float3(bb.pMax.x, bb.pMin.y, bb.pMin.z),
			float3(bb.pMax.x, bb.pMin.y, bb.pMax.z),
			float3(bb.pMax.x, bb.pMax.y, bb.pMin.z),
			float3(bb.pMax.x, bb.pMax.y, bb.pMax.z),
		};

		float3 npMin, npMax, tv;
		for (uint d = 0; d < 8; d++) {
			tv = mul(node.instance.transform, float4(vertices[d], 1)).xyz;
			if (d == 0) {
				npMin = npMax = tv;
			} else {
				npMin = min(npMin, tv);
				npMax = max(npMax, tv);
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