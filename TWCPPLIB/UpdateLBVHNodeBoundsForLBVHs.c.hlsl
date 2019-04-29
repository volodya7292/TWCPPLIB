#include "HLSLHelper.hlsli"

struct InputData {
	uint leaves_offset;
};

RWStructuredBuffer<SceneLBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint i = DTid.x;
	//int* lock = new int[nObjects];
	//memset(lock, 0, nObjects * sizeof(int));

	////Update BB limits
	//for (int i = 0; i < nObjects; i++) {
	uint inode = input.leaves_offset + i;
	uint node = nodes[inode].parent;

	//int oldLock = lock[node];
	//AllMemoryBarrierWithGroupSync();
	//lock[node] += 1;
	//AllMemoryBarrierWithGroupSync();

	//[loop]
	//AllMemoryBarrierWithGroupSync();

	uint leftChild = nodes[node].left_child;
	uint rightChild = nodes[node].right_child;

	if (leftChild == inode || rightChild == uint(-1)) {

		while (true) {
			//if (oldLock == 0)
			//	break;

			leftChild = nodes[node].left_child;
			rightChild = nodes[node].right_child;



			float3 pMin = min(nodes[leftChild].bounds.pMin, nodes[rightChild].bounds.pMin);
			float3 pMax = max(nodes[leftChild].bounds.pMax, nodes[rightChild].bounds.pMax);

			//AllMemoryBarrierWithGroupSync();
			nodes[node].bounds.pMin = pMin;
			nodes[node].bounds.pMax = pMax;

			//if root
			if (nodes[node].parent == uint(-1))
				break;

			node = nodes[node].parent;


		//oldLock = lock[node];
		//AllMemoryBarrierWithGroupSync();
		//lock[node] += 1;
		}
	}
	//AllMemoryBarrierWithGroupSync();
//}

//nodes[i].bounds.pMin = float3(0, 0, 0);
}