#include "HLSLHelper.hlsli"

Texture2D<float4> g_albedo : register(t0);

Texture2D<float4> rt_direct : register(t1);
Texture2D<float4> rt_direct_albedo : register(t2);
Texture2D<float4> rt_indirect : register(t3);
Texture2D<float4> rt_indirect_albedo : register(t4);
Texture2D<float2> g_vrs : register(t5);

sampler sam : register(s0);

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float2 tex_coord: TEXCOORD;
};

// Returns if pixel_pos == center_pixel_pos
inline bool vrs_center_pixel_position(in uint vrs_scale, in uint2 pixel_pos, out uint kernel_size, out float2 new_pixel_pos_floor, out float2 new_pixel_pos) {
	kernel_size = 1 << vrs_scale;
	new_pixel_pos_floor = floor(pixel_pos, kernel_size);
	new_pixel_pos = new_pixel_pos_floor + kernel_size / 2.0f;
	return equals(uint2(floor(new_pixel_pos)), pixel_pos);
}

inline uint2 vrs_pixel_data_pos(in uint2 pixel_pos) {
	float2 vrs = g_vrs[pixel_pos];
	uint kernel;
	uint2 n_pos, n_pos_floor;
	vrs_center_pixel_position(vrs.x, pixel_pos, kernel, n_pos_floor, n_pos);
	return n_pos_floor;
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	uint2 SIZE;
    rt_direct.GetDimensions(SIZE.x, SIZE.y);

	uint2 pixel = input.tex_coord * SIZE;
	float2 vrs = g_vrs[pixel];
	uint kernel;
	uint2 n_pos_floor, n_pos;

	vrs_center_pixel_position(vrs.x, pixel, kernel, n_pos_floor, n_pos);

	uint2 n_pos_top = vrs_pixel_data_pos(uint2(pixel.x, n_pos_floor.y - 1));
	uint2 n_pos_bottom = vrs_pixel_data_pos(uint2(pixel.x, n_pos_floor.y + kernel));
	uint2 n_pos_left = vrs_pixel_data_pos(uint2(n_pos_floor.x - 1, pixel.y));
	uint2 n_pos_right = vrs_pixel_data_pos(uint2(n_pos_floor.x + kernel, pixel.y));

	

	//return float4(1,0,0,1); //the red color

	//float2 texcoord = float2(vI&1, vI>>1);
	//float2 texcoord = float2(pos.x / 2 + 0.5f, -(pos.y) / 2 + 0.5f);

	//return float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);

	//return lerp(rt_result.Sample(sam, input.texCoord), g_albedo.Sample(sam, input.texCoord), 0.5);
	return (rt_direct[n_pos_top] + rt_direct[n_pos_bottom] + rt_direct[n_pos_left] + rt_direct[n_pos_right]) / 4.0f;
	//return g_albedo.Sample(sam, input.texCoord);
}