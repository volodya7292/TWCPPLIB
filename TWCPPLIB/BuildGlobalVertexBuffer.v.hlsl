#include "HLSLHelper.hlsli"

struct InputData {
	uint vertex_offset;
};

RWStructuredBuffer<Vertex> gvb : register(u0);
ConstantBuffer<InputData> input : register(b0);

void main(Vertex vertex, uint vertex_id : SV_VertexID) {
	gvb[input.vertex_offset + vertex_id] = vertex;
}