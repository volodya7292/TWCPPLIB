#include "HLSLHelper.hlsli"

#define PRIMITIVES_PER_THREAD 16

struct VertexMesh {
	uint4 vertex_info;
};

struct InputData {
	uint iteration_count;
	uint element_count;
};

StructuredBuffer<Vertex> gvb : register(t0);
RWStructuredBuffer<Bounds> bounding_box : register(u0);
ConstantBuffer<VertexMesh> vertex_mesh : register(b0);
ConstantBuffer<InputData> input : register(b1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	if (DTid.x * PRIMITIVES_PER_THREAD >= input.element_count) return;

	float3 pMin;
	float3 pMax;

	uint iteration = input.iteration_count;
	uint element_count = input.element_count;

	if (iteration == 0) {
		for (uint i = 0; i < PRIMITIVES_PER_THREAD; i++) {
			uint vert_index = vertex_mesh.vertex_info.x + DTid.x * PRIMITIVES_PER_THREAD * 3 + i * 3;

			if (vert_index >= vertex_mesh.vertex_info.x + vertex_mesh.vertex_info.y)
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
			uint prim_index = DTid.x * PRIMITIVES_PER_THREAD * 3 * iteration + i;

			//if (vert_index >= vertex_mesh.vertex_info.x + vertex_mesh.vertex_info.y)
			//	break;

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