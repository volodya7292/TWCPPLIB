#include "HLSLHelper.hlsli"

static float stack[340];

struct InputData {
	uint iteration;
};

Texture2D<float4> g_position : register(t0);
Texture2D<float4> g_diffuse : register(t1);
Texture2D<float4> g_specular : register(t2);
Texture2D<float4> g_normal : register(t3);

RWTexture2D<float2> output : register(u0);
sampler sam : register(s0);

ConstantBuffer<InputData> input : register(b0);

inline float weight(in float4 position, in float4 diffuse, in float4 normal) {
	if (position.w == 0)
		return -1;
	return luminance(diffuse.rgb) * ((dot(normal.xyz, float3(0, 1, 0)) + 1) / 2.0f);
}

// Kernel size: 32

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 SIZE;
	output.GetDimensions(SIZE.x, SIZE.y);

	uint kernel = 1 << (input.iteration + 1);
	uint2 quad = DTid.xy * kernel;

	float w_avg = 0;
	float w_first = -1;
	float w = 0;

	for (uint x = 0; x < kernel; x += kernel / 2) {
		bool state = false;
		for (uint y = 0; y < kernel; y += kernel / 2) {
			uint2 pos = quad + uint2(x, y);

			if (kernel == 2) {
				float4 position = g_position.SampleLevel(sam, pos / (float2)SIZE, 0);
				float4 diffuse = g_diffuse.SampleLevel(sam, pos / (float2)SIZE, 0);
				//float4 specular = g_specular.SampleLevel(sam, pos / (float2)SIZE, 0);
				float4 normal = g_normal.SampleLevel(sam, pos / (float2)SIZE, 0);

				w = weight(position, diffuse, normal);
				output[pos] = float2(0, w);
			} else {
				if ((uint)round(output[pos].x) != input.iteration)
					w_avg = -1;
				w = output[pos].y;
			}

			if (w_avg == -1 || (w_first != -1 && abs(w_first - w) >= 0.5f)) {
				//x = kernel;
				w_avg = -1;
				state = true;
				break;
			} else if (w_first == -1) {
				w_first = w;
			}

			if (!state)
				w_avg += w * 0.25f;
			//output[pos].y = w;
		}
		if (state)
			break;
	}


	for (x = 0; x < kernel; x++) {
		for (uint y = 0; y < kernel; y++) {
			if (w_avg != -1) {
				//output[quad + uint2(x, y)] = float2(0, output[quad + uint2(x, y)].y);//float2(0, 0);
			//} else {
				output[quad + uint2(x, y)] = float2(input.iteration + 1, w_avg);
			}
		}
	}
}