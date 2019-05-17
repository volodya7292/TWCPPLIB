#include "HLSLHelper.hlsli"

struct InputData {
	uint primitive_count;
	uint leaves_offset;
};

StructuredBuffer<uint4> morton_codes : register(t0);
RWStructuredBuffer<LBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

inline int longest_common_prefix(uint i, uint j) {
	if (j < 0 || j >= input.primitive_count) {
		return -1;
	} else {
		uint mortonCodeA = morton_codes[i].x;
		uint mortonCodeB = morton_codes[j].x;
		if (mortonCodeA != mortonCodeB) {
			return 31 - firstbithigh((unsigned int)(morton_codes[i].x ^ morton_codes[j].x));
			//return __lzcnt((unsigned int)(mortonCodes[i] ^ mortonCodes[j]));
		} else {
			// TODO: Technically this should be primitive ID
			return (31 - firstbithigh(i ^ j)) + 31;
			//return (unsigned int)lzcnt64(i ^ j);// + 31;
		}
	}
}

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	const uint i = DTid.x;

	if (i >= input.leaves_offset)
		return;

	const int sign = longest_common_prefix(i, i + 1) -
		longest_common_prefix(i, i - 1);

	const int d = sign > 0 ? 1 : -1;

	// Compute upper bound for the length of the range
	const int sigMin = longest_common_prefix(i, i - d);
	int lmax = 2;

	while (longest_common_prefix(i, i + lmax * d) > sigMin)
		lmax *= 2;

	// Find the other end using binary search
	int l = 0;
	float divider = 2.0f;
	for (int t = lmax / divider; t >= 1.0f; divider *= 2.0f) {
		if (longest_common_prefix(i, i + (l + t) * d) > sigMin)
			l += t;
		t = lmax / divider;
	}

	const uint j = i + l * d;

	// Find the split position using binary search
	const int sigNode = longest_common_prefix(i, j);
	int s = 0;

	divider = 2.0f;
	for (int t2 = ceil(l / divider); t2 >= 1.0f; divider *= 2.0f) {
		if (longest_common_prefix(i, i + (s + t2) * d) > sigNode)
			s += t2;
		t2 = ceil(l / divider);
	}

	const uint gamma = i + s * d + min(d, 0);

	// Output child pointers
	uint leftChild, rightChild;

	if (min(i, j) == gamma)
		leftChild = input.primitive_count - 1 + gamma;
	else
		leftChild = gamma;

	if (max(i, j) == gamma + 1)
		rightChild = input.primitive_count - 1 + gamma + 1;
	else
		rightChild = gamma + 1;

	nodes[i].left_child = leftChild;
	nodes[i].right_child = rightChild;
	nodes[leftChild].parent = i;
	nodes[rightChild].parent = i;
}