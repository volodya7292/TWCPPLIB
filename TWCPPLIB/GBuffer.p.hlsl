#include "HLSLHelper.hlsli"

Texture2DArray<float4> diffuse_tex : register(t0);
Texture2DArray<float4> specular_tex : register(t1);
Texture2DArray<float4> emission_tex : register(t2);
Texture2DArray<float4> normal_tex : register(t3);

sampler sam : register(s0);

struct PS_OUTPUT {
	float4 position : SV_Target0;    // World space position.  .w component = 0 if a background pixel
	float4 normal   : SV_Target1;    // World space normal.  (.w is distance from camera to hit point; this may not be used)
	float4 diffuse  : SV_Target2;    // .rgb diffuse material color, .a pixel opacity/transparency
	float4 specular : SV_Target3;    // .rgb Falcor's specular representation, .a specular roughness
	float4 emission : SV_Target4;    // .rgb emission material color
	float  depth    : SV_Depth;
};

struct VS_OUTPUT {
	float4        pos : SV_Position;
	float3  tex_coord : TexCoord;
	float3 obj_normal : ObjectNormal;
	float3    tangent : Tangent;
};

ConstantBuffer<Camera> camera : register(b0); // .info.y - scale factor for large objects

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output;

	float3 normal_sample = normal_tex.Sample(sam, input.tex_coord).xyz * 2.0f - 1.0f;
	float3 bitangent = cross(input.obj_normal, input.tangent);
	float3x3 tex_space = float3x3(input.tangent, bitangent, input.obj_normal);
	float3 normal = normalize(mul(normal_sample, tex_space));

	//if (hasNormMap == true) {
	//	//Load normal from normal map
	//	float4 normalMap = ObjNormMap.Sample(ObjSamplerState, input.TexCoord);

	//	//Change normal map range from [0, 1] to [-1, 1]
	//	normalMap = (2.0f*normalMap) - 1.0f;

	//	//Make sure tangent is completely orthogonal to normal
	//	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

	//	//Create the biTangent
	//	float3 biTangent = cross(input.normal, input.tangent);

	//	//Create the "Texture Space"
	//	float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

	//	//Convert normal from normal map to texture space and store in input.normal
	//	input.normal = normalize(mul(normalMap, texSpace));
	//}


	output.position = float4(input.pos.xyz, 1.0f);
	output.normal = float4(normal, 0);
	output.diffuse = diffuse_tex.Sample(sam, input.tex_coord);
	output.specular = specular_tex.Sample(sam, input.tex_coord);
	output.emission = emission_tex.Sample(sam, input.tex_coord);
	output.depth = depth_delinearize(depth_linearize(input.pos.z, 0.1, 1000) / camera.info.y, 0.1, 1000);

	return output;
}