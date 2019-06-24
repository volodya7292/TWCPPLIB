#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

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
RWTexture2D<float4> g_indirect_final_out;

ConstantBuffer<InputData> input_data;

//uniform float kernelWeights[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };
//uniform float gamma            = 0.1;

//struct PS_OUTPUT {
//	float4 direct   : SV_Target0;
//	float4 indirect : SV_Target1;
//};

inline float2 weight(float3 normal, float3 prev_normal,
					float depth, float prev_depth, float depth_max_change,
					float rough, float prev_rough,
					//float lum_dir, float prev_lum_dir, float lum_indir, float prev_lum_indir,
					float pq_length) {

	float w_z = exp(-abs(depth - prev_depth));
	//float w_z = exp(-(abs(depth - prev_depth) / (depth_max_change * pq_length + 0.0001f)));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	float w_r = 1.0f - smoothstep(0.01f, 0.1f, abs(rough - prev_rough));
	float w_d = 1.0f - smoothstep(10.0f * rough, 46.0f * rough, pq_length);

	/*float w_l_dir = saturate(exp(-abs(lum_dir - prev_lum_dir)));
	float w_l_indir = saturate(exp(-abs(lum_indir - prev_lum_indir)));

	float w = w_n * w_z;*/

	return float2(
		w_z * w_n,
		w_z * w_n * w_d
    );
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	uint2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	float2 G_SCALE = G_SIZE / float2(RT_SIZE);

	int2 rt_pixel = DTid.xy;
	int2 g_pixel = rt_pixel * G_SCALE;

	if (greater_equals_any(g_pixel, G_SIZE))
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
	float4 m1_indirect = sumIndirect0 + sumIndirect1;
	float4 m2_direct = sqr(sumDirect);
	float4 m2_indirect = sqr(m1_indirect);
	float  m_count = 1;

	uint iteration_mul = 1 << input_data.iteration;

	// 5x5
	for (int yy = -2; yy <= 2; yy++) {
		for (int xx = -2; xx <= 2; xx++) {
			const int2 p = rt_pixel + float2(xx, yy) * iteration_mul;

			if ((xx != 0 || yy != 0) && greater_equals(p, 0) && less(p, RT_SIZE)) {
				const float kernel = kernelWeights[abs(xx)] * kernelWeights[abs(yy)];

				const float4 directP   = g_direct[p];
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
					//lumDirectCenter, lumDirectP, lumIndirectCenter, lumIndirectP,
					length(float2(xx, yy)));

				sumW   += kernel * w;
				sumDirect   += kernel * w.x * directP;
				sumIndirect0 += kernel * w.x * indirectP0;
				sumIndirect1 += kernel * w.y * indirectP1;

				const float4 indir = indirectP0 + indirectP1;

				m1_direct += directP;
				m1_indirect += indir;
				m2_direct += sqr(directP);
				m2_indirect += sqr(indir);
				m_count++;
			}
		}
	}

	const float4 sn_direct   = sumDirect / sumW.x;
	const float4 sn_indirect0 = sumIndirect0 / sumW.x;
	const float4 sn_indirect1 = sumIndirect1 / sumW.y;
	const float4 sn_indirect = sn_indirect0 + sn_indirect1;

	const float4 sigma_direct   = sqrt(m2_direct / m_count - sqr(m1_direct / m_count));
	const float4 sigma_indirect = sqrt(m2_indirect / m_count - sqr(m1_indirect / m_count));
	const float  gamma          = 0.05f;

	float4 coof_direct = g_detail_sum_direct[rt_pixel] + clamp(sn_direct - g_prev_direct[rt_pixel], -sigma_direct * gamma, sigma_direct * gamma);
	float4 coof_indirect = g_detail_sum_indirect[rt_pixel] + clamp(sn_indirect - g_prev_indirect[rt_pixel], -sigma_indirect * gamma, sigma_indirect * gamma);

	g_detail_sum_direct[rt_pixel] = coof_direct;
	g_detail_sum_indirect[rt_pixel] = coof_indirect;

	if (input_data.iteration == input_data.max_iterations - 1) {
		g_direct_out[rt_pixel] = sn_direct;
		g_indirect_out[rt_pixel] = pack_f2_16(sn_indirect0, sn_indirect1);// - coof_indirect;
		g_indirect_final_out[rt_pixel] = sn_indirect;
	} else {
		g_direct_out[rt_pixel] = sn_direct;
		g_indirect_out[rt_pixel] = pack_f2_16(sn_indirect0, sn_indirect1);
	}
}