#include "HLSLHelper.hlsli"

struct VS_OUTPUT {
	float4           pos : SV_POSITION;
	float2     tex_coord : TEXCOORD;
	linear float3 obj_normal : OBJECTNORMAL;
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
};

struct VertexMeshInstance {
	float4x4 model;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<VertexMeshInstance> vertex_mesh : register(b1);

VS_OUTPUT main(Vertex input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;
	output.pos = mul(mul(camera.proj_view, vertex_mesh.model), float4(input.pos, 1));
	output.tex_coord = input.tex_coord;
	output.obj_normal = input.normal;
	return output;
}