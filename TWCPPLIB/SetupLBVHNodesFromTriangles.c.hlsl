#include "HLSLHelper.hlsli"

struct InputData {
	uint gvb_vertex_offset;
	uint leaves_offset;
	uint node_count;
};

StructuredBuffer<Vertex> gvb : register(t0);
StructuredBuffer<uint4> morton_codes : register(t1);
RWStructuredBuffer<LBVHNode> nodes : register(u0);
//RWStructuredBuffer<int> node_locks : register(u1);
ConstantBuffer<InputData> input : register(b0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	const uint i = DTid.x;

	if (i >= input.node_count)
		return;

	LBVHNode node;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	if (i >= input.leaves_offset) {
		const uint prim_index = morton_codes[i - input.leaves_offset].y; // index
		const uint vert_index = input.gvb_vertex_offset + prim_index * 3;
		node.element_index = prim_index;
		node.bounds.pMin = float4(min(gvb[vert_index].pos, min(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos)), 0);
		node.bounds.pMax = float4(max(gvb[vert_index].pos, max(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos)), 0);
	} else {
		node.element_index = -1;
		node.bounds.pMin = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);
		node.bounds.pMax = float4(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	}

	nodes[i] = node;
}