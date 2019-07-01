#include "HLSLHelper.hlsli"
#include "RTDCommon.hlsli"

struct InputData {
	uint iteration;
	uint max_iterations;
};

Texture2D<float4> g_direct;
Texture2D<uint4> g_indirect;
Texture2D<float4> g_prev_direct;
Texture2D<uint4> g_prev_indirect;
Texture2D<float4> g_compact_data;
RWTexture2D<float4> g_detail_sum_direct;
RWTexture2D<float4> g_detail_sum_indirect;
RWTexture2D<float4> g_direct_out;
RWTexture2D<uint4> g_indirect_out;

Texture2D<float4> g_diffuse;
Texture2D<float4> g_emission;
RWTexture2D<float4> g_final_out;

ConstantBuffer<InputData> input;

inline float2 weight(float3 normal, float3 prev_normal,
					float depth, float prev_depth, float depth_max_change,
					float rough, float prev_rough,
					float pq_length) {

	float w_z = exp(-abs(depth - prev_depth));
	//float w_z = exp(-(abs(depth - prev_depth) / (depth_max_change * pq_length + 0.0001f)));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	float w_r = 1.0f - smoothstep(0.01f, 0.1f, abs(rough - prev_rough));
	float w_d = 1.0f - smoothstep(10.0f * rough, 70.0f * rough, pq_length);

	return float2(
		w_z * w_n,
		min(w_z * w_n * w_d * w_r + 0.1f, 1)
    );
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	int2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	int2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	float2 G_SCALE = G_SIZE / float2(RT_SIZE);

	int2 rt_pixel, g_pixel;
	bool same_pixel = true;
	int radius = 2;
	if (input.iteration == input.max_iterations - 1) {
		rt_pixel = DTid.xy / G_SCALE;
		g_pixel = DTid.xy;
		same_pixel = all(g_pixel == rt_pixel * G_SCALE + G_SCALE / 2);
		radius = 1;
	} else {
		rt_pixel = DTid.xy;
		g_pixel = DTid.xy * G_SCALE + G_SCALE / 2;
	}

	if (any(g_pixel >= G_SIZE))
		return;

	const float kernelWeights[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };

	//float  sumWDirect = 1.0;
	float2 sumW = 1.0;
	float4 sumDirect = g_direct[rt_pixel];
	float4 sumIndirect0, sumIndirect1;
	unpack_f2_16(g_indirect[rt_pixel], sumIndirect0, sumIndirect1);


	float3 normalCenter;
	float2 zCenter;
	float  rCenter;
	fetch_normal_and_linear_z(g_compact_data, g_pixel, normalCenter, zCenter, rCenter);


	float4 m1_direct = sumDirect;
	float4 m1_indirect0 = sumIndirect0;
	float4 m1_indirect1 = sumIndirect1;
	float4 m2_direct = sqr(sumDirect);
	float4 m2_indirect0 = sqr(sumIndirect0);
	float4 m2_indirect1 = sqr(sumIndirect1);
	float  m_count = 1;

	uint iteration_mul = 1 << input.iteration;

	// 5x5)
	for (int yy = -radius; yy <= radius; yy++) {
		for (int xx = -radius; xx <= radius; xx++) {
			const int2 p = rt_pixel + float2(xx, yy) * iteration_mul;

			if ((xx != 0 || yy != 0) && all(p >= 0) && all(p < RT_SIZE)) {
				const float kernel = kernelWeights[abs(xx)] * kernelWeights[abs(yy)];

				const float4 directP = g_direct[p];
				float4 indirectP0, indirectP1;
				unpack_f2_16(g_indirect[p], indirectP0, indirectP1);


				float3 normalP;
				float2 zP;
				float rP;
				fetch_normal_and_linear_z(g_compact_data, p * G_SCALE, normalP, zP, rP);


				// compute the edge-stopping functions
				const float2 w = weight(
					normalP, normalCenter,
					zP.x, zCenter.x, zCenter.y,
					rCenter, rP,
					//centerLum, pLum,
					length(float2(xx, yy) * iteration_mul));

				sumW   += kernel * w;
				sumDirect   += kernel * w.x * directP;
				sumIndirect0 += kernel * w.x * indirectP0;
				sumIndirect1 += kernel * w.y * indirectP1;

				m1_direct += directP;
				m1_indirect0 += indirectP0;
				m1_indirect1 += indirectP1;
				m2_direct += sqr(directP);
				m2_indirect0 += sqr(indirectP0);
				m2_indirect1 += sqr(indirectP1);
				m_count++;
			}
		}
	}

	const float3 sn_direct   = sumDirect.rgb / sumW.x;
	const float3 sn_indirect0 = sumIndirect0.rgb / sumW.x;
	const float3 sn_indirect1 = sumIndirect1.rgb / sumW.y;
	const float3 sn_indirect = sn_indirect0.rgb * sn_indirect1.rgb;

	const float3 sigma_direct   = sqrt(m2_direct.rgb / m_count - sqr(m1_direct.rgb / m_count));
	const float3 sigma_indirect0 = sqrt(m2_indirect0.rgb / m_count - sqr(m1_indirect0.rgb / m_count));
	const float3 sigma_indirect1 = sqrt(m2_indirect1.rgb / m_count - sqr(m1_indirect1.rgb / m_count));
	const float  gamma          = 0.05f;

	float4 sn_prev_indir0, sn_prev_indir1;
	unpack_f2_16(g_prev_indirect[rt_pixel], sn_prev_indir0, sn_prev_indir1);
	float4 detail_indir0, detail_indir1;
	unpack_f2_16(g_detail_sum_indirect[rt_pixel], detail_indir0, detail_indir1);

	float3 coof_direct = g_detail_sum_direct[rt_pixel].rgb + clamp(sn_direct - g_prev_direct[rt_pixel].rgb, -sigma_direct * gamma, sigma_direct * gamma);
	float3 coof_indirect0 = detail_indir0.rgb + clamp(sn_indirect0 - sn_prev_indir0.rgb, -sigma_indirect0 * gamma, sigma_indirect0 * gamma);
	float3 coof_indirect1 = detail_indir1.rgb + clamp(sn_indirect1 - sn_prev_indir1.rgb, -sigma_indirect1 * gamma, sigma_indirect1 * gamma);

	if (same_pixel) {
		g_detail_sum_direct[rt_pixel] = float4(coof_direct, 1);
		g_detail_sum_indirect[rt_pixel] = pack_f2_16(float4(coof_indirect0, 1), float4(coof_indirect1, 1));
	}

	if (input.iteration == input.max_iterations - 1) {
		float4 dir_out = float4(sn_direct - coof_direct, 1);
		
		if (same_pixel) {
			g_direct_out[rt_pixel] = dir_out;
			g_indirect_out[rt_pixel] = pack_f2_16(float4(sn_indirect0 - coof_indirect0, 1), float4(sn_indirect1 - coof_indirect1, 1));
		}

		g_final_out[g_pixel] = g_emission[g_pixel] + (dir_out + float4((sn_indirect0 - coof_indirect0) * (sn_indirect1 - coof_indirect1), 1)) * max(5e-3f, g_diffuse[g_pixel] / PI);
	} else if (same_pixel) {
		g_direct_out[rt_pixel] = float4(sn_direct, 1);
		g_indirect_out[rt_pixel] = pack_f2_16(float4(sn_indirect0, 1), float4(sn_indirect1, 1));
	}
}