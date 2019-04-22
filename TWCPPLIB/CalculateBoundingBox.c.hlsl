#include "HLSLHelper.hlsli"

#define PRIMITIVES_PER_THREAD 16

struct VertexMesh {
	uint4 vertex_info;
};

struct InputData {
	uint4 data; // .x - iteration count, .y - element count
};

StructuredBuffer<Vertex> gvb : register(t0);
RWStructuredBuffer<Bounds> bounding_box : register(u0);
ConstantBuffer<VertexMesh> vertex_mesh : register(b0);
ConstantBuffer<InputData> input : register(b1);

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float3 pMin;
	float3 pMax;

	uint iteration = input.data.x;
	uint element_count = input.data.y;

	if (iteration == 0) {
		for (uint i = 0; i < PRIMITIVES_PER_THREAD; i++) {
			uint prim_index = vertex_mesh.vertex_info.x + DTid.x * PRIMITIVES_PER_THREAD * 3 + i * 3;

			if (prim_index >= vertex_mesh.vertex_info.x + vertex_mesh.vertex_info.y)
				break;

			float3 v0 = gvb[prim_index].pos;
			float3 v1 = gvb[prim_index + 1].pos;
			float3 v2 = gvb[prim_index + 2].pos;

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
			uint prim_index = vertex_mesh.vertex_info.x + DTid.x * PRIMITIVES_PER_THREAD * 3 * iteration + i * 3;

			if (prim_index >= vertex_mesh.vertex_info.x + vertex_mesh.vertex_info.y)
				break;

			Bounds bounds = bounding_box[prim_index];

			if (i == 0) {
				pMin = bounds.pMin;
				pMax = bounds.pMax;
			} else {
				pMin = min(pMin, bounds.pMin);
				pMax = max(pMax, bounds.pMax);
			}
		}
	}

	Bounds bb;
	bb.pMin = pMin;
	bb.pMax = pMax;

	bounding_box[DTid.x] = bb;
}