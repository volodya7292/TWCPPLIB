#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_moments;
Texture2D<float4> g_compact_norm_depth;
Texture2D<float>  g_history_length;

struct PS_OUTPUT {
	float4 direct   : SV_Target0;
	float4 indirect : SV_Target1;
};

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	int2 rt_pixel = input.tex_coord * RT_SIZE;

	PS_OUTPUT output;
	float h = g_history_length[rt_pixel];

	if (h < 4.0) { // not enough temporal history available
		float  sumWDirect   = 0;
		float  sumWIndirect = 0;
		float3 sumDirect    = 0;
		float3 sumIndirect  = 0;
		float4 sumMoments   = 0;

		const float4 directCenter    = g_direct[rt_pixel];
		const float4 indirectCenter  = g_indirect[rt_pixel];
		const float  lDirectCenter   = luminance(directCenter.rgb);
		const float  lIndirectCenter = luminance(indirectCenter.rgb);

		float3 normalCenter;
		float2 zCenter;
		fetch_normal_and_linear_z(g_compact_norm_depth, rt_pixel, normalCenter, zCenter);

		if (zCenter.x < 0) {
			// current pixel does not a valid depth => must be envmap => do nothing
			output.direct   = directCenter;
			output.indirect = indirectCenter;

			return output;
		}

		const float phiDepth     = max(zCenter.y, 1e-8) * 3.0;

		// compute first and second moment spatially. This code also applies cross-bilateral 7x7
		// filtering on the input color samples
		for (int yy = -3; yy <= 3; yy++) {
			for (int xx = -3; xx <= 3; xx++) {
				const int2 p     = rt_pixel + int2(xx, yy);
				const bool inside = greater_equals(p, 0) && less(p, RT_SIZE);
				const bool samePixel = (xx == 0) && (yy == 0);
				const float kernel = 1.0;

				if (inside) {

					const float3 directP   = g_direct[p].rgb;
					const float3 indirectP = g_indirect[p].rgb;
					const float4 momentsP  = g_moments[p];

					const float lDirectP   = luminance(directP.rgb);
					const float lIndirectP = luminance(indirectP.rgb);

					float3 normalP;
					float2 zP;
					fetch_normal_and_linear_z(g_compact_norm_depth, p, normalP, zP);

					const float2 w = computeWeight(
						zCenter.x, zP.x, phiDepth * length(float2(xx, yy)),
						normalCenter, normalP,
						lDirectCenter, lDirectP,
						lIndirectCenter, lIndirectP);

					const float wDirect = w.x;
					const float wIndirect = w.y;

					sumWDirect  += wDirect;
					sumDirect   += directP * wDirect;

					sumWIndirect  += wIndirect;
					sumIndirect   += indirectP * wIndirect;

					sumMoments += momentsP * float4(wDirect.xx, wIndirect.xx);
				}
			}
		}

		// Clamp sums to >0 to avoid NaNs.
		sumWDirect = max(sumWDirect, 1e-6f);
		sumWIndirect = max(sumWIndirect, 1e-6f);

		sumDirect   /= sumWDirect;
		sumIndirect /= sumWIndirect;
		sumMoments  /= float4(sumWDirect.xx, sumWIndirect.xx);

		// compute variance for direct and indirect illumination using first and second moments
		float2 variance = sumMoments.ga - sumMoments.rb * sumMoments.rb;

		// give the variance a boost for the first frames
		variance *= 4.0 / h;

		output.direct = float4(sumDirect, variance.r);
		output.indirect = float4(sumIndirect, variance.g);

		return output;
	} else {
		// do nothing, pass data unmodified
		output.direct = g_direct[rt_pixel];
		output.indirect = g_indirect[rt_pixel];

		return output;
	}
}