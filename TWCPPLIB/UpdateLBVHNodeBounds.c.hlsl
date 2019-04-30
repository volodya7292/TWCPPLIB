#include "HLSLHelper.hlsli"

struct InputData {
	uint leaves_offset;
	uint iteration;
};

RWStructuredBuffer<LBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;

	uint leaf = input.leaves_offset + i;
	uint leaf_parent = nodes[leaf].parent;
	uint tleaf, tleaf_parent;
	uint leftChild, rightChild;
	Bounds lbb, rbb;
	float3 pMin, pMax;

	while (true) {
		if (leaf == nodes[leaf_parent].left_child) {
			leftChild = nodes[leaf_parent].left_child;
			rightChild = nodes[leaf_parent].right_child;
			lbb = nodes[leftChild].bounds;
			rbb = nodes[rightChild].bounds;
			pMin = min(lbb.pMin, rbb.pMin);
			pMax = max(lbb.pMax, rbb.pMax);

			if (not_equals(pMin, -FLT_MAX) && not_equals(pMax, FLT_MAX)) {
				nodes[leaf_parent].bounds.pMin = pMin;
				nodes[leaf_parent].bounds.pMax = pMax;
			} else {
				break;
			}

			leaf = leaf_parent;
			leaf_parent = nodes[leaf_parent].parent;
		} else {
			break;
		}
	}
}