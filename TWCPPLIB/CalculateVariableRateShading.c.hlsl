#include "HLSLHelper.hlsli"

struct InputData {
	uint iteration;
};

Texture2D<float4> g_position : register(t0);
Texture2D<float4> g_diffuse : register(t1);
Texture2D<float4> g_specular : register(t2);
Texture2D<float4> g_normal : register(t3);

RWTexture2D<float> output : register(u0);
sampler sam : register(s0);

ConstantBuffer<InputData> input : register(b0);


inline float weight(in float4 position, in float4 diffuse) {
	return position.w * diffuse.r;
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 SIZE;
	output.GetDimensions(SIZE.x, SIZE.y);

	uint2 quad = DTid.xy * 32;

	uint k = 32 >> input.iteration; // 32  16  8  4
	uint s = 32 / k;                // 1   2   4  8
	uint k2 = 1 << input.iteration;


	for (uint x = 0; x < k; x++) {
		for (uint y = 0; y < k; y++) {

			uint2 p = uint2(x, y);
			uint2 pt = quad + p * s;

			if (pt.x >= SIZE.x || pt.y >= SIZE.y)
				continue;

			float w;
			float w_last = -1;
			float w_sum = 0;
			float c = 0;

			for (uint x2 = 0; x2 < 2; x2++) {
				for (uint y2 = 0; y2 < 2; y2++) {
					uint2 ps = pt + uint2(x2, y2) * s;

					if (ps.x >= SIZE.x || ps.y >= SIZE.y)
						continue;

					float2 tex_coord = (float2)ps / (float2)SIZE;

					if (input.iteration == 0) {
						float4 position = g_position.SampleLevel(sam, tex_coord, 0);
						float4 diffuse = g_diffuse.SampleLevel(sam, tex_coord, 0);
						//float4 specular = g_specular.SampleLevel(sam, tex_coord, 0);
						//float4 normal = g_normal.SampleLevel(sam, tex_coord, 0);
						w = weight(position, diffuse);
					} else {
						w = output[ps];
					}

					if (w_last != -1 && abs(w_last - w) >= 0.5f) {
						x2 = 2;
						w_last = -1;
						break;
					}

					w_last = w;
					w_sum += w;
					c++;
				}
			}

			if (w_last == -1) {
				w = -1;
				output[pt] = 0;
			} else {
				output[pt] = w_sum / c;
			}
		}
	}
}