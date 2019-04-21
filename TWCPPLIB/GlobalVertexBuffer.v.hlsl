struct VS_INPUT {
	float3       pos : POSITION;
	float2 tex_coord : TEXCOORD;
	float3    normal : NORMAL;
};

struct PerVertexBuffer {
	uint4 vertex_offset;
};

ConstantBuffer<PerVertexBuffer> vertex_buffer : register(b0);
RWStructuredBuffer<VS_INPUT> gvb : register(u0);

void main(VS_INPUT input, uint vertex_id : SV_VertexID)
{
	gvb[vertex_buffer.vertex_offset.x + vertex_id] = input;
}