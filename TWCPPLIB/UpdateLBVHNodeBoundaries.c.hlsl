#include "HLSLHelper.hlsli"

struct InputData {
	uint leaves_offset;
};

globallycoherent RWStructuredBuffer<LBVHNode> nodes : register(u0);
globallycoherent RWStructuredBuffer<int> lock : register(u1);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint i = DTid.x;
	//int* lock = new int[nObjects];
	//memset(lock, 0, nObjects * sizeof(int));

	////Update BB limits
	//for (int i = 0; i < nObjects; i++) {
		uint node = nodes[input.leaves_offset + i].parent;

		int oldLock = lock[node];
		lock[node] += 1;

		//[loop]
		while (true) {
			if (oldLock == 0)
				break;

			uint leftChild = nodes[node].left_child;
			uint rightChild = nodes[node].right_child;

			float3 pMin = min(nodes[leftChild].bounds.pMin, nodes[rightChild].bounds.pMin);
			float3 pMax = max(nodes[leftChild].bounds.pMax, nodes[rightChild].bounds.pMax);

			nodes[node].bounds.pMin = pMin;
			nodes[node].bounds.pMax = pMax;

			//if root
			if (nodes[node].parent == uint(-1))
				break;

			node = nodes[node].parent;

			oldLock = lock[node];
			lock[node] += 1;
		}
	//}

	//nodes[i].bounds.pMin = float3(0, 0, 0);
}