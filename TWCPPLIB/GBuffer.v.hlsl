#include "HLSLHelper.hlsli"

struct VS_OUTPUT {
	float4           clip_pos : SV_Position;
	linear float3   world_pos : ModelPosition;
	float2          tex_coord : TexCoord;
	uint          material_id : MaterialId;
	linear float3  obj_normal : ObjectNormal;
	linear float3     tangent : Tangent;
};

struct VertexMeshInstance {
	float4x4 model;
	float4x4 model_reduced;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<VertexMeshInstance> vertex_mesh : register(b1);

VS_OUTPUT main(Vertex input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;

	float4x4 model;
	if (camera.info.y == 1.0f)
		model = vertex_mesh.model;
	else
		model = vertex_mesh.model_reduced;
	
	output.world_pos = mul(model, float4(input.pos, 1)).xyz;
	output.clip_pos = mul(camera.proj_view, float4(output.world_pos, 1));
	output.tex_coord = input.tex_coord.xy;
	output.material_id = uint(input.tex_coord.z);
	output.obj_normal = mul(float4(input.normal, 1), model).xyz;
	output.tangent = mul(float4(input.tangent, 1), model).xyz;

	return output;
}