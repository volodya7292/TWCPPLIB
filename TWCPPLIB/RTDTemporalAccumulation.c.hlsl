#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_filt_direct;
Texture2D<float4> g_filt_indirect;
Texture2D<float4> g_motion;
Texture2D<float4> g_compact_data;
Texture2D<float4> g_prev_compact_data;
RWTexture2D<float4> g_direct_out;
RWTexture2D<float4> g_indirect_out;

//struct PS_OUTPUT {
//	float4 direct         : SV_Target0;
//	float4 indirect       : SV_Target1;
//};

inline float weight(float3 normal, float3 prev_normal, float depth, float prev_depth, float depth_max_change, float pq_length) {
	float w_z = exp(-abs(depth - prev_depth));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	return w_z * w_n;
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	uint2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	const int2 rt_pixel = DTid.xy;
	if (greater_equals_any(rt_pixel, RT_SIZE))
		return;

	const float2 G_SCALE = G_SIZE / float2(RT_SIZE);
	const int2 g_pixel = rt_pixel * G_SCALE;

	const float4 motion = g_motion[g_pixel];
	const int2 rt_pixel_prev = int2(rt_pixel + motion.xy * float2(RT_SIZE) + float2(0.5f, 0.5f));
	const int2 g_pixel_prev = rt_pixel_prev * G_SCALE;


	float3 normalP;
	float2 zP;
	fetch_normal_and_linear_z(g_compact_data, g_pixel, normalP, zP);
	float3 prev_normalP;
	float2 prev_zP;
	fetch_normal_and_linear_z(g_prev_compact_data, g_pixel_prev, prev_normalP, prev_zP);


	const float4 direct = g_direct[rt_pixel];
	const float4 indirect = g_indirect[rt_pixel];
	const float4 prev_direct = g_filt_direct[rt_pixel_prev];
	const float4 prev_indirect = g_filt_indirect[rt_pixel_prev];

	const float alpha = 0.95f * weight(normalP, prev_normalP, zP.x, prev_zP.x, zP.y, length(rt_pixel - rt_pixel_prev));

	g_direct_out[rt_pixel] = lerp(direct, prev_direct, alpha);
	g_indirect_out[rt_pixel] = lerp(indirect, prev_indirect, alpha);
}