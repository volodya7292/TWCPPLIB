#include "HLSLHelper.hlsli"

struct PerVertexBuffer {
	uint4 vertex_offset;
};

ConstantBuffer<PerVertexBuffer> vertex_buffer : register(b0);
RWStructuredBuffer<Vertex> gvb : register(u0);

void main(Vertex input, uint vertex_id : SV_VertexID) {
	gvb[vertex_buffer.vertex_offset.x + vertex_id] = input;
}