#include "HLSLHelper.hlsli"

struct VS_OUTPUT {
	float4             clip_pos : SV_Position;
	float4        prev_clip_pos : PrevPosition;
	linear float3     world_pos : WorldPosition;
	float2            tex_coord : TexCoord;
	uint            material_id : MaterialId;
	//linear float3        normal : ObjectNormal;
	linear float3  world_normal : WorldNormal;
	linear float3       tangent : Tangent;
};

struct VertexMeshInstance {
	float4x4 model;
	float4x4 prev_model;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<Camera> prev_camera : register(b1);
ConstantBuffer<VertexMeshInstance> vertex_mesh : register(b2);

void reduce_model(inout float4x4 model, float reduce_scale) {
	float3 to_mesh = translation(model) - camera.pos.xyz;
	float3 mesh_pos_new = camera.pos.xyz + normalize(to_mesh) * (length(to_mesh) * reduce_scale);

	model = translate(model, mesh_pos_new);
}

VS_OUTPUT main(Vertex input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;

	float4x4 model = vertex_mesh.model;
	float4x4 prev_model = vertex_mesh.prev_model;

	float reduce_scale = camera.info.y;
	if (reduce_scale != 1.0f) {
		reduce_model(model, reduce_scale);
		reduce_model(prev_model, reduce_scale);
	}
	
	output.world_pos = mul(model, float4(input.pos, 1)).xyz;
	output.clip_pos = mul(camera.proj_view, float4(output.world_pos, 1));
	output.tex_coord = input.tex_coord.xy;
	output.material_id = uint(input.tex_coord.z);
	//output.normal = input.normal;
	output.world_normal = mul(float4(input.normal, 1), model).xyz;
	output.tangent = mul(float4(input.tangent, 1), model).xyz;

	output.world_normal = dot(output.world_normal, normalize(camera.pos.xyz - output.world_pos)) >= 0 ? output.world_normal : -output.world_normal;

	output.prev_clip_pos = mul(prev_camera.proj_view, mul(prev_model, float4(input.pos, 1)));

	return output;
}