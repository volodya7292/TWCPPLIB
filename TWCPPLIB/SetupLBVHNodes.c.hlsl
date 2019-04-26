#include "HLSLHelper.hlsli"

struct InputData {
	uint gvb_vertex_offset;
	uint leaves_offset;
};

StructuredBuffer<Vertex> gvb : register(t0);
StructuredBuffer<uint> morton_codes : register(t1);
StructuredBuffer<uint> morton_code_indices : register(t2);
RWStructuredBuffer<LBVHNode> nodes : register(u0);
//RWStructuredBuffer<int> node_locks : register(u1);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;

	LBVHNode node;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	if (i >= input.leaves_offset) {
		uint prim_index = morton_code_indices[i - input.leaves_offset];
		uint vert_index = input.gvb_vertex_offset + prim_index * 3;
		node.primitive_index = prim_index;
		node.bounds.pMin = min(gvb[vert_index].pos, min(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));
		node.bounds.pMax = max(gvb[vert_index].pos, max(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));
		//node_locks[i - input.leaves_offset] = 0;
	} else {
		node.primitive_index = -1;
		node.bounds.pMin = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		node.bounds.pMax = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	nodes[i] = node;
}