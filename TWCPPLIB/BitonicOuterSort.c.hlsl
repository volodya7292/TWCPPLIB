#include "BitonicSortCommon.hlsli"

RWByteAddressBuffer g_SortBuffer : register(u0);
RWByteAddressBuffer g_IndexBuffer : register(u1);

struct InputData {
	uint k;    // k >= 4096
	uint j;    // j >= 2048 && j < k
};

ConstantBuffer<InputData> Constants : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	// Form unique index pair from dispatch thread ID
	uint Index2 = InsertOneBit(DTid.x, Constants.j);
	uint Index1 = Index2 ^ (Constants.k == 2 * Constants.j ? Constants.k - 1 : Constants.j);

	if (Index2 >= CB1.ListCount)
		return;

	uint A = g_SortBuffer.Load(Index1 * 4);
	uint B = g_SortBuffer.Load(Index2 * 4);
	uint indexA = g_IndexBuffer.Load(Index1 * 4);
	uint indexB = g_IndexBuffer.Load(Index2 * 4);
	if (ShouldSwap(A, B, indexA, indexB)) {
		g_SortBuffer.Store(Index1 * 4, B);
		g_SortBuffer.Store(Index2 * 4, A);


		g_IndexBuffer.Store(Index1 * 4, indexB);
		g_IndexBuffer.Store(Index2 * 4, indexA);
	}
}