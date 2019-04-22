#include "HLSLHelper.hlsli"

struct VertexMesh {
	uint4 vertex_info;
};

struct InputData {
	uint4 element_count;
};

globallycoherent RWStructuredBuffer<uint> morton_codes : register(u0);
globallycoherent RWStructuredBuffer<uint> morton_code_indices : register(u1);
ConstantBuffer<InputData> input : register(b0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint element_count = input.element_count.x;

	uint curr_element = morton_codes[DTid.x];
	uint min_element = -1;

	uint min_element_index, code;
	for (uint i = DTid.x; i < element_count; i++) {
		code = morton_codes[i];
		if (code < min_element) {
			min_element = code;
			min_element_index = i;
		}
	}

	// TODO: FIX WRITES BY SORTING IN ANOTHER BUFFER

	if (min_element < curr_element) {
		morton_codes[DTid.x] = min_element;
		//morton_codes[min_element_index] = curr_element;
		uint curr_code_index = morton_code_indices[DTid.x];
		morton_code_indices[DTid.x] = min_element_index;
		//morton_code_indices[min_element_index] = curr_code_index;
	}
}