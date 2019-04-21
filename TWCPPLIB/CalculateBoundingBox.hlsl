#include "HLSLHelper.hlsli"

struct VertexMesh {
	uint4 vertex_info;
};

StructuredBuffer<Vertex> gvb : register(t0);
globallycoherent RWStructuredBuffer<Bounds> bounding_box : register(u0);
ConstantBuffer<VertexMesh> vertex_mesh : register(b0);

bool less(float3 v0, float3 v1) {
	return v0.x < v1.x && v0.y < v1.y && v0.z < v1.z;
}

bool greater(float3 v0, float3 v1) {
	return v0.x > v1.x && v0.y > v1.y && v0.z > v1.z;
}

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float3 vertex = gvb[vertex_mesh.vertex_info.x + DTid.x].pos;
	if (less(vertex, bounding_box[0].pMin))
		bounding_box[0].pMin = vertex;
	else if (greater(vertex, bounding_box[0].pMax))
		bounding_box[0].pMax = vertex;
}