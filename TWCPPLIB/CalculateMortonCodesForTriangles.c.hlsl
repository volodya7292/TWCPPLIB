#include "HLSLHelper.hlsli"

struct InputData {
	uint gvb_vertex_offset;
	uint gvb_vertex_count;
};

StructuredBuffer<Vertex> gvb : register(t0);
StructuredBuffer<Bounds> bounding_box : register(t1);
RWStructuredBuffer<uint4> morton_codes : register(u0);
//RWStructuredBuffer<uint> morton_code_indices : register(u1);
ConstantBuffer<InputData> input : register(b0);

inline uint expandBits(uint v) {
	v = (v * 0x00010001u) & 0xFF0000FFu;
	v = (v * 0x00000101u) & 0x0F00F00Fu;
	v = (v * 0x00000011u) & 0xC30C30C3u;
	v = (v * 0x00000005u) & 0x49249249u;
	return v;
}
inline unsigned int morton3D(float3 center) {
	center.x = min(max(center.x * 1024.0f, 0.0f), 1023.0f);
	center.y = min(max(center.y * 1024.0f, 0.0f), 1023.0f);
	center.z = min(max(center.z * 1024.0f, 0.0f), 1023.0f);

	uint xx = expandBits(uint(center.x));
	uint yy = expandBits(uint(center.y));
	uint zz = expandBits(uint(center.z));
	return (xx << 2) + (yy << 1) + zz;
}

inline float3 computeCenter(float3 cmin, float3 cmax, float3 min, float3 max) {
	float3 tmpMin, tmpMax;
	float3 len = cmax - cmin;

	tmpMin = (min - cmin) / len;
	tmpMax = (max - cmin) / len;

	float3 axis = tmpMax - tmpMin;
	float d = length(axis) * 0.5f;

	axis = normalize(axis);
	return tmpMin + d * axis;
}

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	const uint vert_index = input.gvb_vertex_offset + DTid.x * 3;

	if (vert_index >= input.gvb_vertex_offset + input.gvb_vertex_count)
		return;

	const float3 Cmin = bounding_box[0].pMin.xyz;
	const float3 Cmax = bounding_box[0].pMax.xyz;

	const float3 pMin = min(gvb[vert_index].pos, min(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));
	const float3 pMax = max(gvb[vert_index].pos, max(gvb[vert_index + 1].pos, gvb[vert_index + 2].pos));

	const float3 center = computeCenter(Cmin, Cmax, pMin, pMax);
	const uint code = morton3D(center);

	morton_codes[DTid.x].x = code;
	morton_codes[DTid.x].y = DTid.x;
}