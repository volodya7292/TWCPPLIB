#include "HLSLHelper.hlsli"
#include "SVGFCommon.hlsli"

Texture2D<float4> g_direct;
Texture2D<float4> g_indirect;
Texture2D<float4> g_prev_direct;
Texture2D<float4> g_prev_indirect;
Texture2D<float4> g_prev_moments;
Texture2D<float4> g_motion;
Texture2D<float4> g_linear_z;
Texture2D<float4> g_prev_linear_z;
Texture2D<float>  g_history_length;

struct PS_OUTPUT {
	float4 direct         : SV_Target0;
	float4 indirect       : SV_Target1;
	float4 moments        : SV_Target2;
	float  history_length : SV_Target3;
};

bool isReprjValid(uint2 image_size, int2 coord, float Z, float Zprev, float fwidthZ, float3 normal, float3 normalPrev, float fwidthNormal) {
	if (
		(less_any(coord, 1) || greater_equals_any(coord, image_size)) ||    // check whether reprojected pixel is inside of the screen
		(abs(Zprev - Z) / (fwidthZ + 1e-4) > 2.0) ||                        // check if deviation of depths is acceptable
		(distance(normal, normalPrev) / (fwidthNormal + 1e-2) > 16.0)       // check normals for compatibility
	)
		return false;
	else
		return true;
}

bool loadPrevData(in uint2 rt_size, in uint2 rt_pixel, out float4 prev_direct, out float4 prev_indirect, out float4 prev_moments, out float history_length) {
	// xy = motion, z = length(fwidth(pos)), w = length(fwidth(normal))
	float4 motion = g_motion[rt_pixel];

	// +0.5 to account for texel center offset
	const int2 rt_pixel_prev = int2(float2(rt_pixel) + motion.xy * rt_size + float2(0.5, 0.5));

	// stores: Z, fwidth(z), z_prev
	float4 depth  = g_linear_z[rt_pixel];
	float3 normal = oct_to_dir(asuint(depth.w));

	prev_direct   = float4(0, 0, 0, 0);
	prev_indirect = float4(0, 0, 0, 0);
	prev_moments  = float4(0, 0, 0, 0);

	bool v[4];
	const float2 posPrev = rt_pixel + motion.xy * rt_size;
	const int2 offset[4] = { int2(0, 0), int2(1, 0), int2(0, 1), int2(1, 1) };

	// check for all 4 taps of the bilinear filter for validity
	bool valid = false;
	for (int sampleIdx = 0; sampleIdx < 4; sampleIdx++) {
		int2 loc = int2(posPrev) + offset[sampleIdx];
		float4 depthPrev  = g_prev_linear_z[loc];
		float3 normalPrev = oct_to_dir(asuint(depthPrev.w));

		v[sampleIdx] = isReprjValid(rt_size, rt_pixel_prev, depth.z, depthPrev.x, depth.y, normal, normalPrev, motion.w);
		valid = valid || v[sampleIdx];
	}

	if (valid) {
		float sumw = 0;
		float x = frac(posPrev.x);
		float y = frac(posPrev.y);

		// bilinear weights
		float w[4] = {
			(1 - x) * (1 - y),
			x * (1 - y),
			(1 - x) * y,
			x * y
		};

		prev_direct   = 0;
		prev_indirect = 0;
		prev_moments  = 0;

		// perform the actual bilinear interpolation
		for (int sampleIdx = 0; sampleIdx < 4; sampleIdx++) {
			int2 loc = int2(posPrev) + offset[sampleIdx];

			if (v[sampleIdx]) {
				prev_direct   += w[sampleIdx] * g_prev_direct[loc];
				prev_indirect += w[sampleIdx] * g_prev_indirect[loc];
				prev_moments  += w[sampleIdx] * g_prev_moments[loc];
				sumw         += w[sampleIdx];
			}
		}

		// redistribute weights in case not all taps were used
		valid = (sumw >= 0.01);
		prev_direct   = valid ? prev_direct / sumw   : float4(0, 0, 0, 0);
		prev_indirect = valid ? prev_indirect / sumw : float4(0, 0, 0, 0);
		prev_moments  = valid ? prev_moments / sumw  : float4(0, 0, 0, 0);
	}
	if (!valid) { // perform cross-bilateral filter in the hope to find some suitable samples somewhere
		float cnt = 0.0;

		// this code performs a binary descision for each tap of the cross-bilateral filter 3x3
		for (int yy = -1; yy <= 1; yy++) {
			for (int xx = -1; xx <= 1; xx++) {
				int2 p = rt_pixel_prev + int2(xx, yy);
				float4 depthFilter  = g_prev_linear_z[p];
				float3 normalFilter = oct_to_dir(asuint(depthFilter.w));

				if (isReprjValid(rt_size, rt_pixel_prev, depth.z, depthFilter.x, depth.y, normal, normalFilter, motion.w)) {
					prev_direct   += g_prev_direct[p];
					prev_indirect += g_prev_indirect[p];
					prev_moments  += g_prev_moments[p];
					cnt += 1.0;
				}
			}
		}

		if (cnt > 0) {
			valid = true;
			prev_direct   /= cnt;
			prev_indirect /= cnt;
			prev_moments  /= cnt;
		}
	}

	if (valid) {
		history_length = g_history_length[rt_pixel_prev];
	} else {
		prev_direct   = float4(0, 0, 0, 0);
		prev_indirect = float4(0, 0, 0, 0);
		prev_moments  = float4(0, 0, 0, 0);
		history_length = 0;
	}

	return valid;
}

PS_OUTPUT main(VS_QUAD input) {
	uint2 RT_SIZE;
	g_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);

	uint2 rt_pixel = input.tex_coord * RT_SIZE;


	float3 direct = g_direct[rt_pixel].rgb;
	float3 indirect = g_indirect[rt_pixel].rgb;
	
	float historyLength;
	float4 prevDirect, prevIndirect, prevMoments;
	bool success = loadPrevData(RT_SIZE, rt_pixel, prevDirect, prevIndirect, prevMoments, historyLength);
	historyLength = min(32.0f, success ? historyLength + 1.0f : 1.0f);

	// this adjusts the alpha for the case where insufficient history is available.
	// It boosts the temporal accumulation to give the samples equal weights in
	// the beginning.
	const float alpha        = success ? max(0.05f, 1.0 / historyLength) : 1.0;
	const float alphaMoments = success ? max(0.2f, 1.0 / historyLength) : 1.0;

	// compute first two moments of luminance
	float4 moments;
	moments.r = luminance(direct);
	moments.b = luminance(indirect);
	moments.g = moments.r * moments.r;
	moments.a = moments.b * moments.b;

	// temporal integration of the moments
	moments = lerp(prevMoments, moments, alphaMoments);

	PS_OUTPUT output;

	output.moments = moments;
	output.history_length = historyLength;

	float2 variance = max(float2(0, 0), moments.ga - moments.rb * moments.rb);

	// temporal integration of direct and indirect illumination
	output.direct   = lerp(prevDirect, float4(direct, 0), alpha);
	output.indirect = lerp(prevIndirect, float4(indirect, 0), alpha);

	// variance is propagated through the alpha channel
	output.direct.a = variance.r;
	output.indirect.a = variance.g;

	return output;
}