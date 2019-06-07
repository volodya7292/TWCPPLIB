#include "HLSLHelper.hlsli"

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
	return position.w == 0 ? -1 : (luminance(diffuse.rgb) * ((dot(normal.xyz, float3(0, 1, 0)) + 1) / 2.0f));
}

// Kernel size: 32

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 SIZE;
	output.GetDimensions(SIZE.x, SIZE.y);

	const uint kernel = 1 << (input.iteration + 1);
	const uint2 quad = DTid.xy * kernel;

	if (quad.x >= SIZE.x || quad.y >= SIZE.y)
		return;

	float w_avg = 0;
	float w_first = -1;
	float w = 0;
	float4 position;
	float4 diffuse;
	float4 normal;

	bool state = false;

	uint x, y;
	uint2 pos;
	float2 tex_coord;

	for (x = 0; x < kernel; x += kernel / 2) {
		for (y = 0; y < kernel; y += kernel / 2) {
			pos = quad + float2(x, y);

			if (pos.x >= SIZE.x || pos.y >= SIZE.y)
				continue;

			tex_coord = pos / float2(SIZE);

			if (kernel == 2) {
				position = g_position.SampleLevel(sam, tex_coord, 0);
				diffuse = g_diffuse.SampleLevel(sam, tex_coord, 0);
				//float4 specular = g_specular.SampleLevel(sam, pos / (float2)SIZE, 0);
				normal = g_normal.SampleLevel(sam, tex_coord, 0);

				w = weight(position, diffuse, normal);
				output[pos] = float2(0, w);
			} else if (uint(output[pos].x) != input.iteration) {
				w_avg = -1;
				state = true;
				break;
			} else {
				w = output[pos].y;
			}

			if (w_avg == -1 || (w_first != -1 && abs(w_first - w) >= 0.5f)) {
				w_avg = -1;
				state = true;
				break;
			} else if (w_first == -1) {
				w_first = w;
			}

			if (!state)
				w_avg += w * 0.25f;
		}

		if (state)
			break;
	}

	uint2 kernel2 = min(SIZE - quad, kernel);

	if (w_avg != -1)
		for (x = 0; x < kernel2.x; x++)
			for (y = 0; y < kernel2.y; y++)
				output[quad + uint2(x, y)] = float2(input.iteration + 1, w_avg);
}