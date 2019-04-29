#include "HLSLHelper.hlsli"

struct InputData {
	uint primitive_count;
};

StructuredBuffer<uint> morton_codes : register(t0);
StructuredBuffer<uint> morton_code_indices : register(t1);
RWStructuredBuffer<SceneLBVHNode> nodes : register(u0);
ConstantBuffer<InputData> input : register(b0);

inline int longest_common_prefix(uint i, uint j) {
	if (j < 0 || j >= input.primitive_count) {
		return -1;
	} else {
		uint mortonCodeA = morton_codes[i];
		uint mortonCodeB = morton_codes[j];
		if (mortonCodeA != mortonCodeB) {
			return 31 - firstbithigh((unsigned int)(morton_codes[i] ^ morton_codes[j]));
			//return __lzcnt((unsigned int)(mortonCodes[i] ^ mortonCodes[j]));
		} else {
			// TODO: Technically this should be primitive ID
			return (31 - firstbithigh(i ^ j)) + 31;
			//return (unsigned int)lzcnt64(i ^ j);// + 31;
		}
	}
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	int i = DTid.x;
	uint nObjects = input.primitive_count;

	//build BVH
//#pragma omp parallel for 
	//for (int i = 0; i < nObjects - 1; i++) {
		// Determine direction of the range (+1 or -1)
	int sign = longest_common_prefix(i, i + 1) -
		longest_common_prefix(i, i - 1);

	int d = sign > 0 ? 1 : -1;

	// Compute upper bound for the length of the range
	int sigMin = longest_common_prefix(i, i - d);
	int lmax = 2;

	while (longest_common_prefix(i, i + lmax * d) > sigMin) {
		lmax *= 2;
	}

	// Find the other end using binary search
	int l = 0;
	float divider = 2.0f;
	for (int t = lmax / divider; t >= 1.0f; divider *= 2.0f) {
		if (longest_common_prefix(i, i + (l + t) * d) > sigMin) {
			l += t;
		}
		t = lmax / divider;
	}

	int j = i + l * d;

	// Find the split position using binary search
	int sigNode = longest_common_prefix(i, j);
	int s = 0;

	divider = 2.0f;
	for (int t2 = ceil(l / divider); t2 >= 1.0f; divider *= 2.0f) {
		if (longest_common_prefix(i, i + (s + t2) * d) > sigNode) {
			s += t2;
		}
		t2 = ceil(l / divider);
	}

	int gamma = i + s * d + min(d, 0);

	// Output child pointers
	SceneLBVHNode current = nodes[i];

	if (min(i, j) == gamma) {
		current.left_child = nObjects - 1 + gamma;
	} else {
		current.left_child = gamma;
	}

	if (max(i, j) == gamma + 1) {
		current.right_child = nObjects - 1 + gamma + 1;
	} else {
		current.right_child = gamma + 1;
	}

	nodes[current.left_child].parent = i;
	nodes[current.right_child].parent = i;

	nodes[i].left_child = current.left_child;
	nodes[i].right_child = current.right_child;
	//nodes[i].bounds.pMin = float3(0, 5, 3);
//}

}