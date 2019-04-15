struct VS_INPUT {
	float4       pos : POSITION;
	float2 tex_coord : TEXCOORD;
	float3    normal : NORMAL;
};

struct VS_OUTPUT {
	float4           pos : SV_POSITION;
	float2     tex_coord : TEXCOORD;
	linear float3 normal : OBJECTNORMAL;
};

struct Camera {
	float4x4 proj_view;
};

struct PerObject {
	float4x4 model;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<PerObject> object : register(b1);

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.pos = mul(mul(camera.proj_view, object.model), input.pos);
	output.tex_coord = input.tex_coord;
	output.normal = input.normal;
	return output;
}