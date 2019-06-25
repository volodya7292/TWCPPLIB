#include "HLSLHelper.hlsli"

struct InputData {
	uint leaves_offset;
	uint leaf_count;
};

RWStructuredBuffer<LBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	const uint i = DTid.x;

	if (input.leaves_offset + i >= input.leaf_count)
		return;

	uint leaf = input.leaves_offset + i;
	uint leaf_parent = nodes[leaf].parent;
	uint leftChild = nodes[leaf_parent].left_child;
	uint rightChild = nodes[leaf_parent].right_child;
	Bounds lbb, rbb;
	float3 pMin, pMax;

	[loop]
	while (leaf == nodes[leaf_parent].left_child) {
		Bounds bbl = nodes[leftChild].bounds;
		Bounds bbr = nodes[rightChild].bounds;

		if (leftChild != -1 && rightChild != -1) {
			pMin = min(bbl.pMin.xyz, bbr.pMin.xyz);
			pMax = max(bbl.pMax.xyz, bbr.pMax.xyz);
		} else if (leftChild != -1) {
			pMin = bbl.pMin.xyz;
			pMax = bbl.pMax.xyz;
		} else if (rightChild != -1) {
			pMin = bbr.pMin.xyz;
			pMax = bbr.pMax.xyz;
		}

		if (all(pMin != -FLT_MAX) && all(pMax != FLT_MAX)) {
			nodes[leaf_parent].bounds.pMin = float4(pMin, 0);
			nodes[leaf_parent].bounds.pMax = float4(pMax, 0);

			leaf = leaf_parent;
			leaf_parent = nodes[leaf_parent].parent;
		}

		if (leaf_parent == -1)
			break;

		leftChild = nodes[leaf_parent].left_child;
		rightChild = nodes[leaf_parent].right_child;
	}
}