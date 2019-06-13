#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_filt_direct;
Texture2D<float4> g_filt_indirect;
Texture2D<float4> g_motion;
Texture2D<float4> g_compact_data;
Texture2D<float4> g_prev_compact_data;

struct PS_OUTPUT {
	float4 direct         : SV_Target0;
	float4 indirect       : SV_Target1;
};

inline float weight(float3 normal, float3 prev_normal, float depth, float prev_depth, float depth_max_change, float pq_length) {
	float w_z = exp(-abs(depth - prev_depth));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	return w_z * w_n;
}

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	uint2 G_SIZE;
	g_prev_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);


	int2 rt_pixel = input.tex_coord * RT_SIZE;
	int2 g_pixel = rt_pixel * (G_SIZE / float2(RT_SIZE));

	float4 motion = g_motion[g_pixel];
	int2 rt_pixel_prev = int2(rt_pixel + motion.xy * float2(RT_SIZE) + float2(0.5f, 0.5));
	int2 g_pixel_prev = float2(rt_pixel_prev) * (G_SIZE / float2(RT_SIZE));


	float3 normalP;
	float2 zP;
	fetch_normal_and_linear_z(g_compact_data, g_pixel, normalP, zP);
	float3 prev_normalP;
	float2 prev_zP;
	fetch_normal_and_linear_z(g_prev_compact_data, g_pixel_prev, prev_normalP, prev_zP);


	float4 direct = g_direct[rt_pixel];
	float4 indirect = g_indirect[rt_pixel];
	float4 prev_direct = g_filt_direct[rt_pixel_prev];
	float4 prev_indirect = g_filt_indirect[rt_pixel_prev];

	float alpha = 0.95f * weight(normalP, prev_normalP, zP.x, prev_zP.x, zP.y, length(rt_pixel - rt_pixel_prev));

	PS_OUTPUT output;
	output.direct = lerp(direct, prev_direct, alpha);
	output.indirect = lerp(indirect, prev_indirect, alpha);


	return output;
}