#include "HLSLHelper.hlsli"
#include "GBufferCommon.hlsli"

Texture2DArray<float4> diffuse_tex;
Texture2DArray<float4> specular_tex;
Texture2DArray<float4> emission_tex;
Texture2DArray<float4> normal_tex;

StructuredBuffer<Material> materials;

sampler linear_sampler;

struct GSOutput {
	uint               rt_index : SV_RenderTargetArrayIndex;
	float4             clip_pos : SV_Position;
	//float4        prev_clip_pos : PrevPosition;
	linear float3     world_pos : WorldPosition;
	float2            tex_coord : TexCoord;
	uint            material_id : MaterialId;
	linear float3  world_normal : WorldNormal;
	linear float3       tangent : Tangent;
};

struct PSOutput {
	float4 position    : SV_Target0;    // World space position, .w = 0 if a background pixel
	float4 normal      : SV_Target1;    // World space normal
	float4 diffuse     : SV_Target2;    // .rgb diffuse material color, .a pixel opacity/transparency
	float4 specular    : SV_Target3;    // .rgb specular, .a specular roughness
	float4 emission    : SV_Target4;    // .rgb emission material color
	float  depth       : SV_Depth;
};

ConstantBuffer<Camera> camera; 
ConstantBuffer<RendererInfo> renderer; // .info.y - scale factor for large objects

// Take current clip position, last frame pixel position and compute a motion vector
float2 calcMotionVector(float4 prevClipPos, float2 currentPixelTexPos) {
	float2 prevPosNDC = (prevClipPos.xy / prevClipPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
	float2 motionVec  = prevPosNDC - currentPixelTexPos;

	// Guard against inf/nan due to projection by w <= 0.
	const float epsilon = 1e-5f;
	motionVec = (prevClipPos.w < epsilon) ? float2(0, 0) : motionVec;
	return motionVec;
}

float4 evaluate_texture_linear(Texture2DArray tex, float2 tex_coord, uint id0, uint id1, float blend_factor, float4 default_value) {
	if ((id0 == -1 && id1 != -1) || (blend_factor == 0.0f && id1 != -1))
		return tex.Sample(linear_sampler, float3(tex_coord, id1));
	else if ((id0 != -1 && id1 == -1) || (blend_factor == 1.0f && id0 != -1))
		return tex.Sample(linear_sampler, float3(tex_coord, id0));
	else if (id0 != -1 && id1 != -1)
		return lerp(tex.Sample(linear_sampler, float3(tex_coord, id0)), tex.Sample(linear_sampler, float3(tex_coord, id1)), blend_factor);
	else
		return default_value;
}

float4 evaluate_texture_exact(Texture2DArray tex, float2 tex_coord, uint id0, uint id1, float blend_factor, float4 default_value) {
	if ((id0 == -1 && id1 != -1) || (blend_factor == 0.0f && id1 != -1))
		return tex[float3(tex_coord, id1)];
	else if ((id0 != -1 && id1 == -1) || (blend_factor == 1.0f && id0 != -1))
		return tex[float3(tex_coord, id0)];
	else if (id0 != -1 && id1 != -1)
		return lerp(tex[float3(tex_coord, id0)], tex[float3(tex_coord, id1)], blend_factor);
	else
		return default_value;
}

void evaluate_material(uint id, float2 tex_coord, float3 world_normal, float3 tangent, out float4 diffuse, out float4 specular, out float4 emission, out float3 normal) {
	Material mat = materials[id];

	diffuse = evaluate_texture_linear(diffuse_tex, tex_coord, mat.texture_ids0[0], mat.texture_ids1[0], mat.emission.a, mat.diffuse);
	specular = evaluate_texture_linear(specular_tex, tex_coord, mat.texture_ids0[1], mat.texture_ids1[1], mat.emission.a, mat.specular);
	emission = evaluate_texture_linear(emission_tex, tex_coord, mat.texture_ids0[2], mat.texture_ids1[2], mat.emission.a, mat.emission);

	float3 tex_normal = evaluate_texture_exact(normal_tex, tex_coord, mat.texture_ids0[3], mat.texture_ids1[3], mat.emission.a, 0).xyz;
	
	if (all(tex_normal == 0)) {
		normal = world_normal;
	} else {
		tex_normal = tex_normal * 2.0f - 1.0f;
		tangent = normalize(tangent - dot(tangent, world_normal) * world_normal);

		float3 bitangent = cross(world_normal, tangent);
		float3x3 tex_space = float3x3(tangent, bitangent, world_normal);

		normal = normalize(mul(tex_normal, tex_space));
	}
}

PSOutput main(GSOutput input) {
	//uint2 pixel = input.clip_pos.xy * uint2(renderer.info.x, renderer.info.y);
	//rand_init(pixel.x + pixel.y * renderer.info.x, renderer.info.z, 16);

	float4 diffuse, specular, emission;
	float3 normal;
	evaluate_material(input.material_id, input.tex_coord, input.world_normal, input.tangent, diffuse, specular, emission, normal);

	bool large_scale = renderer.info.y != 1;


	PSOutput output;
	output.position = float4(input.world_pos.xyz, POSITION_FLAG_POS_EXIST | (large_scale ? POSITION_FLAG_LARGE_SCALE : 0));
	output.normal = float4(normal, 0);
	output.diffuse = diffuse;
	output.specular = specular;
	output.emission = emission;
	output.depth = distance(input.world_pos.xyz, camera.pos.xyz) / (large_scale ? renderer.info.y : 1) / 8192.0f;

	return output;
}