#include "HLSLHelper.hlsli"

struct VS_OUTPUT {
	float4               pos : SV_Position;
	float2         tex_coord : TexCoord;
	linear float3 obj_normal : ObjectNormal;
	linear float vmi_scale_factor : VMIScaleFactor;
};

struct VertexMeshInstance {
	float4x4 model;
	float4x4 model_reduced;
	float4 info; // .x - scale factor (large objects are scaled down)
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<VertexMeshInstance> vertex_mesh : register(b1);

VS_OUTPUT main(Vertex input, uint vertex_id : SV_VertexID) {
	VS_OUTPUT output;
	output.obj_normal = input.normal;

	if (vertex_mesh.info.x == 1.0f)
		output.pos = mul(mul(camera.proj_view, vertex_mesh.model), float4(input.pos, 1));
	else
		output.pos = mul(mul(camera.proj_view, vertex_mesh.model_reduced), float4(input.pos, 1));
	
	output.tex_coord = input.tex_coord;
	output.obj_normal = input.normal;
	output.vmi_scale_factor = vertex_mesh.info.x;

	return output;
}