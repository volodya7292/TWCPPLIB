Texture2DArray<float4> diffuseTex : register(t0);
Texture2DArray<float4> specularTex : register(t1);

sampler sam : register(s0);

struct PS_OUTPUT {
	float4 position : SV_Target0;    // World space position.  .w component = 0 if a background pixel
	float4 normal   : SV_Target1;    // World space normal.  (.w is distance from camera to hit point; this may not be used)
	float4 diffuse  : SV_Target2;    // .rgb diffuse material color, .a pixel opacity/transparency
	float4 specular : SV_Target3;    // .rgb Falcor's specular representation, .a specular roughness
	float  depth    : SV_Depth;
};

struct VS_OUTPUT {
	float4               pos : SV_Position;
	float2         tex_coord : TexCoord;
	linear float3 obj_normal : ObjectNormal;
	linear float vmi_scale_factor : VMIScaleFactor;
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
};

ConstantBuffer<Camera> camera : register(b0);

float linearize(float d, float zNear, float zFar) {
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

float unlinearize(float ld, float zNear, float zFar) {
	return (zNear * zFar / ld - zFar) / (zNear - zFar);
}

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output;

	output.position = float4(input.pos.xyz, 1);
	output.normal = float4(input.obj_normal, 0);
	output.diffuse = diffuseTex.Sample(sam, float3(input.tex_coord, 0));
	output.specular = specularTex.Sample(sam, float3(input.tex_coord, 0));
	output.depth = unlinearize(linearize(input.pos.z, 0.1, 1000) / input.vmi_scale_factor, 0.1, 1000);


	return output;
}