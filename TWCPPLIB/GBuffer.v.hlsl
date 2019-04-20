struct VS_INPUT {
	float4       pos : POSITION;
	float2 tex_coord : TEXCOORD;
	float3    normal : NORMAL;
};

struct VS_OUTPUT {
	float4           pos : SV_POSITION;
	float2     tex_coord : TEXCOORD;
	linear float3 obj_normal : OBJECTNORMAL;
};

struct Camera {
	float4x4 proj_view;
};

struct PerVertexMesh {
	float4x4 model;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<PerVertexMesh> vertex_mesh : register(b1);

VS_OUTPUT main(VS_INPUT input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;
	output.pos = mul(mul(camera.proj_view, vertex_mesh.model), input.pos);
	output.tex_coord = input.tex_coord;
	output.obj_normal = input.normal;
	return output;
}