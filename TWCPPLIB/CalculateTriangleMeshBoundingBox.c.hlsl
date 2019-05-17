#include "HLSLHelper.hlsli"

#define PRIMITIVES_PER_THREAD 16

struct InputData {
	uint gvb_vertex_offset;
	uint vertex_count;
	uint iteration_count;
	uint element_count;
};

StructuredBuffer<Vertex> gvb : register(t0);
RWStructuredBuffer<Bounds> bounding_box : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float3 pMin;
	float3 pMax;

	uint gvb_vertex_offset = input.gvb_vertex_offset;
	uint vertex_count = input.vertex_count;
	uint iteration = input.iteration_count;
	uint element_count = input.element_count;

	if (iteration == 0) {
		for (uint i = 0; i < PRIMITIVES_PER_THREAD; i++) {
			uint vert_index = gvb_vertex_offset + DTid.x * PRIMITIVES_PER_THREAD * 3 + i * 3;

			if (vert_index >= gvb_vertex_offset + vertex_count)
				break;

			float3 v0 = gvb[vert_index].pos;
			float3 v1 = gvb[vert_index + 1].pos;
			float3 v2 = gvb[vert_index + 2].pos;

			if (i == 0) {
				pMin = min(v0, min(v1, v2));
				pMax = max(v0, max(v1, v2));
			} else {
				pMin = min(pMin, min(v0, min(v1, v2)));
				pMax = max(pMax, max(v0, max(v1, v2)));
			}
		}
	} else {
		for (uint i = 0; i < PRIMITIVES_PER_THREAD; i++) {
			uint prim_index = DTid.x * PRIMITIVES_PER_THREAD * iteration + i;

			if (prim_index >= input.element_count)
				break;

			Bounds bounds = bounding_box[prim_index];

			if (i == 0) {
				pMin = bounds.pMin.xyz;
				pMax = bounds.pMax.xyz;
			} else {
				pMin = min(pMin, bounds.pMin.xyz);
				pMax = max(pMax, bounds.pMax.xyz);
			}
		}
	}

	Bounds bb;
	bb.pMin = float4(pMin, 0);
	bb.pMax = float4(pMax, 0);

	bounding_box[DTid.x] = bb;
}