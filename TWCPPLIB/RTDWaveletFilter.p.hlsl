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

	return w_z * w_n;
}

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	int2 rt_pixel = input.tex_coord * RT_SIZE;

	const int   gStepSize        = 1 << input_data.iteration;
	const float kernelWeights[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };

	// explicitly store/accumulate center pixel with weight 1 to prevent issues
	// with the edge-stopping functions
	float  sumW = 1.0;
	//float  sumWIndirect = 1.0;
	float4 sumDirect    = g_direct[rt_pixel];//directCenter;
	float4 sumIndirect  = g_indirect[rt_pixel];//indirectCenter;

	float3 normalCenter;
	float2 zCenter;
	fetch_normal_and_linear_z(g_compact_data, rt_pixel, normalCenter, zCenter);

	float4 m1_direct = sumDirect;
	float4 m1_indirect = sumIndirect;
	float4 m2_direct = sumDirect * sumDirect;
	float4 m2_indirect = sumIndirect * sumIndirect;
	float  m_count = 1;

	// 5x5
	for (int yy = -2; yy <= 2; yy++) {
		for (int xx = -2; xx <= 2; xx++) {
			const int2 p      = rt_pixel + int2(xx, yy) * gStepSize;
			const bool inside = greater_equals(p, 0) && less(p, RT_SIZE);

			const float kernel = kernelWeights[abs(xx)] * kernelWeights[abs(yy)];

			if (inside && (xx != 0 || yy != 0)) { // skip center pixel, it is already accumulated
				const float4 directP     = g_direct[p];
				const float4 indirectP   = g_indirect[p];

				float3 normalP;
				float2 zP;
				fetch_normal_and_linear_z(g_compact_data, p, normalP, zP);

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
	float4 bound_min_direct = mu_direct - sigma_direct;
	float4 bound_max_direct = mu_direct + sigma_direct;
	float4 bound_min_indirect = mu_indirect - sigma_indirect;
	float4 bound_max_indirect = mu_indirect + sigma_indirect;

	float4 coofs_direct = g_detail_sum_direct[rt_pixel];
	float4 coofs_indirect = g_detail_sum_indirect[rt_pixel];
	coofs_direct += clamp(sn_direct - g_prev_direct[rt_pixel], bound_min_direct, bound_max_direct);
	coofs_indirect += clamp(sn_indirect - g_prev_indirect[rt_pixel], bound_min_indirect, bound_max_indirect);

	g_detail_sum_direct[rt_pixel] = coofs_direct;
	g_detail_sum_indirect[rt_pixel] = coofs_indirect;


	PS_OUTPUT output;

	if (input_data.iteration == input_data.max_iterations - 1) {
		output.direct   = sn_direct - coofs_direct;
		output.indirect = sn_indirect - coofs_indirect;
	} else {
		output.direct   = sumDirect / sumW;
		output.indirect = sumIndirect / sumW;
	}

	return output;
}