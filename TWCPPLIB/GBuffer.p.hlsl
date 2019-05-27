#include "HLSLHelper.hlsli"

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
};

ConstantBuffer<Camera> camera : register(b0); // .info.x - scale factor for large objects

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output;

	output.position = float4(input.pos.xyz, 1);
	output.normal = float4(input.obj_normal, 0);
	output.diffuse = diffuseTex.Sample(sam, float3(input.tex_coord, 0));
	output.specular = specularTex.Sample(sam, float3(input.tex_coord, 0));
	output.depth = depth_delinearize(depth_linearize(input.pos.z, 0.1, 1000) / camera.info.x, 0.1, 1000);


	return output;
}