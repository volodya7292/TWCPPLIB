#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

struct InputData {
	uint iteration;
};

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_compact_norm_depth;
Texture2D<float>  g_history_length;

ConstantBuffer<InputData> input_data;

struct PS_OUTPUT {
	float4 direct    : SV_Target0;
	float4 indirect  : SV_Target1;
};

// computes a 3x3 gaussian blur of the variance, centered around the current pixel
float2 compute_variance_center(int2 pixel) {
	float2 sum = 0;

	const float kernel[2][2] = {
		{ 1.0 / 4.0, 1.0 / 8.0 },
		{ 1.0 / 8.0, 1.0 / 16.0 }
	};

	// 3x3
	for (int yy = -1; yy <= 1; yy++) {
		for (int xx = -1; xx <= 1; xx++) {
			int2 p = pixel + int2(xx, yy);
			float k = kernel[abs(xx)][abs(yy)];

			sum.r += g_direct[p].a * k;
			sum.g += g_indirect[p].a * k;
		}
	}

	return sum;
}

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	int2 rt_pixel = input.tex_coord * RT_SIZE;


	const float epsVariance      = 1e-10;
	const float kernelWeights[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };

	// constant samplers to prevent the compiler from generating code which
	// fetches the sampler descriptor from memory for each texture access
	const float4 directCenter    = g_direct[rt_pixel];
	const float4 indirectCenter  = g_indirect[rt_pixel];
	const float  lDirectCenter   = luminance(directCenter.rgb);
	const float  lIndirectCenter = luminance(indirectCenter.rgb);

	// variance for direct and indirect, filtered using 3x3 gaussin blur
	const float2 var = compute_variance_center(rt_pixel);

	// number of temporally integrated pixels
	const float historyLength = g_history_length[rt_pixel];

	float3 normalCenter;
	float2 zCenter;
	fetch_normal_and_linear_z(g_compact_norm_depth, rt_pixel, normalCenter, zCenter);

	PS_OUTPUT output;

	if (zCenter.x < 0) { // not a valid depth => must be envmap => do not filter
		output.direct   = directCenter;
		output.indirect = indirectCenter;

		return output;
	}

	const int   gStepSize    = 1 << input_data.iteration;
	const float phiLDirect   = 10.0f * sqrt(max(0.0, epsVariance + var.r));
	const float phiLIndirect = 10.0f * sqrt(max(0.0, epsVariance + var.g));
	const float phiDepth     = max(zCenter.y, 1e-8) * gStepSize;

	// explicitly store/accumulate center pixel with weight 1 to prevent issues
	// with the edge-stopping functions
	float  sumWDirect   = 1.0;
	float  sumWIndirect = 1.0;
	float4 sumDirect    = directCenter;
	float4 sumIndirect  = indirectCenter;

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
				fetch_normal_and_linear_z(g_compact_norm_depth, p, normalP, zP);
				const float lDirectP   = luminance(directP.rgb);
				const float lIndirectP = luminance(indirectP.rgb);

				// compute the edge-stopping functions
				const float2 w = computeWeight(
					zCenter.x, zP.x, phiDepth * length(float2(xx, yy)),
					normalCenter, normalP,
					lDirectCenter, lDirectP, phiLDirect,
					lIndirectCenter, lIndirectP, phiLIndirect);

				const float wDirect   = w.x * kernel;
				const float wIndirect = w.y * kernel;

				// alpha channel contains the variance, therefore the weights need to be squared, see paper for the formula
				sumWDirect  += wDirect;
				sumDirect   += float4(wDirect.xxx, wDirect * wDirect) * directP;

				sumWIndirect  += wIndirect;
				sumIndirect   += float4(wIndirect.xxx, wIndirect * wIndirect) * indirectP;
			}
		}
	}

	// renormalization is different for variance, check paper for the formula
	output.direct   = float4(sumDirect   / float4(sumWDirect.xxx, sumWDirect * sumWDirect));
	output.indirect = float4(sumIndirect / float4(sumWIndirect.xxx, sumWIndirect * sumWIndirect));

	return output;
}