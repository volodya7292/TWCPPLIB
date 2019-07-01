#include "HLSLHelper.hlsli"
#include "RTDCommon.hlsli"

Texture2D<float4> g_direct;
Texture2D<uint4> g_indirect;
Texture2D<float4> g_filt_direct;
Texture2D<uint4> g_filt_indirect;
Texture2D<float2> g_motion;
Texture2D<float4> g_compact_data;
Texture2D<float4> g_prev_compact_data;
RWTexture2D<float4> g_direct_out;
RWTexture2D<uint4> g_indirect_out;

inline float weight(float3 normalCenter, float3 normalP, float depthCenter, float depthP) {
	float w_z = clamp(exp(-abs(depthCenter - depthP)), 0, 1);
	float w_n = pow(max(0.0f, dot(normalCenter, normalP)), 32.0f);

	return w_z * w_n;
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	int2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	int2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	const int2 rt_pixel = DTid.xy;
	if (any(rt_pixel >= RT_SIZE))
		return;

	const float2 G_SCALE = G_SIZE / float2(RT_SIZE);
	const int2 g_pixel = rt_pixel * G_SCALE + G_SCALE / 2;

	const float2 motion = g_motion[g_pixel];
	const int2 rt_pixel_prev = int2(rt_pixel + motion.xy * float2(RT_SIZE) + float2(0.5f, 0.5f));
	const int2 g_pixel_prev = rt_pixel_prev * G_SCALE + G_SCALE / 2.0f;


	float sample_w = 0;


	float3 normalCenter;
	float2 depthCenter;
	fetch_normal_and_linear_z(g_compact_data, g_pixel, normalCenter, depthCenter);

	float3 normalP = 0;
	float2 depthP = 0;
	if (all(rt_pixel_prev >= 0) && all(rt_pixel_prev < RT_SIZE)) {
		fetch_normal_and_linear_z(g_prev_compact_data, g_pixel_prev, normalP, depthP);
		sample_w = 1;
	}


	// Load currect pixel
	const float4 direct = g_direct[rt_pixel];
	float4 indirect0, indirect1;
	unpack_f2_16(g_indirect[rt_pixel], indirect0, indirect1);

	// Load previous pixel
	const float4 prev_direct = g_filt_direct[rt_pixel];
	float4 prev_indirect0, prev_indirect1;
	unpack_f2_16(g_filt_indirect[rt_pixel], prev_indirect0, prev_indirect1);



	if (sample_w == 1)
		sample_w = weight(normalCenter, normalP, depthCenter.x, depthP.x);



	const float alpha = 0.95 * clamp(sample_w, 0.1, 1);

	g_direct_out[rt_pixel] = lerp(direct, prev_direct, alpha);
	g_indirect_out[rt_pixel] = pack_f2_16(lerp(indirect0, prev_indirect0, alpha), lerp(indirect1, prev_indirect1, alpha));
}