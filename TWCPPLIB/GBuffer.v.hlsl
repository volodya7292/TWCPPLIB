#include "HLSLHelper.hlsli"

struct VS_OUTPUT {
	float4             clip_pos : SV_Position;
	float4        prev_clip_pos : PrevPosition;
	linear float3     world_pos : WorldPosition;
	float2            tex_coord : TexCoord;
	uint            material_id : MaterialId;
	linear float3        normal : ObjectNormal;
	linear float3  world_normal : WorldNormal;
	linear float3       tangent : Tangent;
};

struct VertexMeshInstance {
	float4x4 model;
	float4x4 model_reduced;
	float4x4 prev_model;
	float4x4 prev_model_reduced;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<Camera> prev_camera : register(b1);
ConstantBuffer<VertexMeshInstance> vertex_mesh : register(b2);

VS_OUTPUT main(Vertex input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;

	float4x4 model, prev_model;
	if (camera.info.y == 1.0f) {
		model = vertex_mesh.model;
		prev_model = vertex_mesh.model;
	} else {
		model = vertex_mesh.model_reduced;
		prev_model = vertex_mesh.model_reduced;
	}
	
	output.world_pos = mul(model, float4(input.pos, 1)).xyz;
	output.clip_pos = mul(camera.proj_view, float4(output.world_pos, 1));
	output.tex_coord = input.tex_coord.xy;
	output.material_id = uint(input.tex_coord.z);
	output.normal = input.normal;
	output.world_normal = mul(float4(input.normal, 1), model).xyz;
	output.tangent = mul(float4(input.tangent, 1), model).xyz;

	output.prev_clip_pos = mul(prev_camera.proj_view, mul(prev_model, float4(input.pos, 1)));

	return output;
}