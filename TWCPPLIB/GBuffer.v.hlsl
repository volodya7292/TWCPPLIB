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

	/*if (vertex_mesh.info.y == 1.0f) {
		float3 to_mesh = translation(vertex_mesh.model) - camera.pos.xyz;
		float3 mesh_pos_new = normalize(to_mesh) * (length(to_mesh) * vertex_mesh.info.x);
		
		float4x4 model_new = scale(vertex_mesh.model, vertex_mesh.info.x);
		model_new = translate(model_new, mesh_pos_new);

		output.pos = mul(mul(camera.proj_view, model_new), float4(input.pos, 1));
	}*/

	output.pos = mul(mul(camera.proj_view, vertex_mesh.model), float4(input.pos, 1));

	output.tex_coord = input.tex_coord;
	output.obj_normal = input.normal;
	
	return output;
}