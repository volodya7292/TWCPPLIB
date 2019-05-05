#include "HLSLHelper.hlsli"

struct InputData {
	uint leaves_offset;
};

RWStructuredBuffer<SceneLBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;

	uint leaf = input.leaves_offset + i;
	uint leaf_parent = nodes[leaf].parent;
	uint leftChild, rightChild;
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

		if (pMin.x != -FLT_MAX && pMax.x != FLT_MAX) {
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