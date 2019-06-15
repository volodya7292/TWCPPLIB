#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

struct InputData {
	uint iteration;
	uint max_iterations;
};

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_prev_direct;
Texture2D<float4> g_prev_indirect;
Texture2D<float4> g_compact_data;
RWTexture2D<float4> g_detail_sum_direct;
RWTexture2D<float4> g_detail_sum_indirect;

ConstantBuffer<InputData> input_data;

struct PS_OUTPUT {
	float4 direct   : SV_Target0;
	float4 indirect : SV_Target1;
};

float weight(float3 normal, float3 prev_normal, float depth, float prev_depth, float depth_max_change, float pq_length) {
	//float w_z = exp(-abs(depth - prev_depth));
	float w_z = exp(-(abs(depth - prev_depth) / (depth_max_change * pq_length + 0.0001f)));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	return w_n;
}

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	uint2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	float2 G_SCALE = G_SIZE / float2(RT_SIZE);

	int2 rt_pixel = input.tex_coord * RT_SIZE;

	const int   gStepSize        = 1 << input_data.iteration;
	const float kernelWeights[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };

	// explicitly store/accumulate center pixel with weight 1 to prevent issues
	// with the edge-stopping functions
	float  sumW = 0.0;
	//float  sumWIndirect = 1.0;
	float4 sumDirect    = 0;//g_direct[rt_pixel];//directCenter;
	float4 sumIndirect  = 0;//g_indirect[rt_pixel];//indirectCenter;

	float3 normalCenter;
	float2 zCenter;
	fetch_normal_and_linear_z(g_compact_data, rt_pixel * G_SCALE, normalCenter, zCenter);

	float4 m1_direct = 0;//sumDirect;
	float4 m1_indirect = 0;//sumIndirect;
	float4 m2_direct = 0;//sumDirect * sumDirect;
	float4 m2_indirect = 0;//sumIndirect * sumIndirect;
	float  m_count = 0;

	// 5x5
	for (int yy = -2; yy <= 2; yy++) {
		for (int xx = -2; xx <= 2; xx++) {
			const int2 p      = rt_pixel + int2(xx, yy) * gStepSize;
			const bool inside = greater_equals(p, 0) && less(p, RT_SIZE);

			const float kernel = kernelWeights[abs(xx)] * kernelWeights[abs(yy)];

			if (inside) {
				const float4 directP     = g_direct[p];
				const float4 indirectP   = g_indirect[p];

				float3 normalP;
				float2 zP;
				fetch_normal_and_linear_z(g_compact_data, p * G_SCALE, normalP, zP);

				// compute the edge-stopping functions
				const float w = weight(normalP, normalCenter, zP.x, zCenter.x, zCenter.y, length(float2(xx, yy)));

				sumW += kernel * w;
				sumDirect += kernel * w * directP;
				sumIndirect += kernel * w * indirectP;

				m1_direct += directP;
				m1_indirect += indirectP;
				m2_direct += directP * directP;
				m2_indirect += indirectP * indirectP;
				m_count++;
			}
		}
	}

	float4 sn_direct   = sumDirect / sumW;
	float4 sn_indirect = sumIndirect / sumW;

	float4 mu_direct = m1_direct / m_count;
	float4 mu_indirect = m1_indirect / m_count;
	float4 sigma_direct = sqrt(m2_direct / m_count - mu_direct * mu_direct);
	float4 sigma_indirect = sqrt(m2_indirect / m_count - mu_indirect * mu_indirect);
	float gamma = 0.1;
	float4 bound_min_direct = -sigma_direct * gamma;
	float4 bound_max_direct = sigma_direct * gamma;
	float4 bound_min_indirect = -sigma_indirect * gamma;
	float4 bound_max_indirect = sigma_indirect * gamma;

	float4 coof_direct = g_detail_sum_direct[rt_pixel];
	float4 coof_indirect = g_detail_sum_indirect[rt_pixel];
	float4 prev_direct = g_prev_direct[rt_pixel];
	float4 prev_indirect = g_prev_indirect[rt_pixel];
	float4 curr_coof_direct = clamp(sn_direct - prev_direct, bound_min_direct, bound_max_direct);
	float4 curr_coof_indirect = clamp(sn_indirect - prev_indirect, bound_min_indirect, bound_max_indirect);
	coof_direct += curr_coof_direct;
	coof_indirect += curr_coof_indirect;

	g_detail_sum_direct[rt_pixel] = coof_direct;
	g_detail_sum_indirect[rt_pixel] = coof_indirect;


	PS_OUTPUT output;

	/*output.direct   = sn_direct - curr_coof_direct;
	output.indirect = sn_indirect - curr_coof_indirect;*/

	if (input_data.iteration == input_data.max_iterations - 1) {
		output.direct   = sn_direct - coof_direct;
		output.indirect = sn_indirect - coof_indirect;
	} else {
		output.direct   = sn_direct;
		output.indirect = sn_indirect;
	}

	return output;
}