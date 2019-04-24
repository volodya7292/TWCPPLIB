#include "HLSLHelper.hlsli"

struct InputData {
	uint gvb_vertex_offset;
	uint leaves_offset;
};

StructuredBuffer<Vertex> gvb : register(t0);
StructuredBuffer<uint> morton_codes : register(t1);
StructuredBuffer<uint> morton_code_indices : register(t2);
RWStructuredBuffer<LBVHNode> nodes : register(u0);
ConstantBuffer<InputData> constants : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	int i = DTid.x;
	uint code_index = morton_code_indices[i];


	uint vert_index = constants.gvb_vertex_offset + i * 3;
	float3 pMin = min(gvb[vert_index].pos, min(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));
	float3 pMax = max(gvb[vert_index].pos, max(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));

	nodes[i].bounds.pMin = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	nodes[i].bounds.pMax = float3(FLT_MAX, FLT_MAX, FLT_MAX);

	LBVHNode node;
	node.primitive_index = i;
	node.bounds.pMin = pMin;
	node.bounds.pMax = pMax;
	node.parent = -1;
	node.left_child = -1;
	node.right_child = -1;

	nodes[constants.leaves_offset + i] = node;

	/*int aux = 0;
	unsigned int value;
	for (auto entry : mcodes) {
		mortonCodes[aux] = entry.first;
		value = entry.second;

		nodes[leafOffset + aux] = temp[value];

		cout << entry.first << " " << entry.second << endl;
		aux++;
	}*/
}