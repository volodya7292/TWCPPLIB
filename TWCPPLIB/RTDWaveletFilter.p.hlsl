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

float weight(float3 normal, float3 prev_normal, float depth, float prev_depth, float depth_max_change, float rough, float prev_rough, float pq_length) {
	float w_z = exp(-abs(depth - prev_depth));
	//float w_z = exp(-(abs(depth - prev_depth) / (depth_max_change * pq_length + 0.0001f)));
	float w_n = pow(max(0.0f, dot(normal, prev_normal)), 32.0f);

	//float w_r = 0.01;//1.0f - smoothstep(0.01f, 0.1f, abs(rough - prev_rough));

	return w_n * w_z;
}

PS_OUTPUT main(VS_QUAD input) {
	//uint2 RT_SIZE;
	//g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	uint2 G_SIZE;
	g_compact_data.GetDimensions(G_SIZE.x, G_SIZE.y);

	//float2 G_SCALE = G_SIZE / float2(RT_SIZE);

	//int2 rt_pixel = input.tex_coord * RT_SIZE;
	int2 g_pixel = input.tex_coord * G_SIZE;

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
	float  rCenter;
	fetch_normal_and_linear_z(g_compact_data, g_pixel, normalCenter, zCenter, rCenter);

	float4 m1_direct = 0;//sumDirect;
	float4 m1_indirect = 0;//sumIndirect;
	float4 m2_direct = 0;//sumDirect * sumDirect;
	float4 m2_indirect = 0;//sumIndirect * sumIndirect;
	float  m_count = 0;

	int2 p;
	bool inside;
	float kernel, rP, w;
	float4 directP, indirectP;
	float3 normalP;
	float2 zP;


	// 5x5
	[unroll]
	for (int yy = -2; yy <= 2; yy++) {
		[unroll]
		for (int xx = -2; xx <= 2; xx++) {
			p      = g_pixel + int2(xx, yy) * gStepSize;
			inside = greater_equals(p, 0) && less(p, G_SIZE);

			kernel = kernelWeights[abs(xx)] * kernelWeights[abs(yy)];

			if (inside) {
				directP     = g_direct[p];
				indirectP   = g_indirect[p];

				fetch_normal_and_linear_z(g_compact_data, p, normalP, zP,rP);

				// compute the edge-stopping functions
				w = weight(normalP, normalCenter, zP.x, zCenter.x, zCenter.y, rP, rCenter, length(float2(xx, yy)));

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

	const float4 sn_direct   = sumDirect / sumW;
	const float4 sn_indirect = sumIndirect / sumW;

	const float4 sigma_direct = sqrt(m2_direct / m_count - sqr(m1_direct / m_count));
	const float4 sigma_indirect = sqrt(m2_indirect / m_count - sqr(m1_indirect / m_count));
	const float gamma = 0.1;

	float4 coof_direct = g_detail_sum_direct[g_pixel];
	float4 coof_indirect = g_detail_sum_indirect[g_pixel];

	coof_direct += clamp(sn_direct - g_prev_direct[g_pixel], -sigma_direct * gamma, sigma_direct * gamma);
	coof_indirect += clamp(sn_indirect - g_prev_indirect[g_pixel], -sigma_indirect * gamma, sigma_indirect * gamma);

	g_detail_sum_direct[g_pixel] = coof_direct;
	g_detail_sum_indirect[g_pixel] = coof_indirect;


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