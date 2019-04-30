#include "HLSLHelper.hlsli"

#define ELEMENTS_PER_THREAD 16

struct InputData {
	uint gnb_offset_count;
	uint iteration_count;
	uint element_count;
};

StructuredBuffer<LBVHNode> gnb : register(t0);
StructuredBuffer<SceneLBVHInstance> gnb_offsets : register(t1);
RWStructuredBuffer<Bounds> bounding_box : register(u0);
ConstantBuffer<InputData> input : register(b0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float3 pMin;
	float3 pMax;

	uint gnb_offset_count = input.gnb_offset_count;
	uint iteration = input.iteration_count;
	uint element_count = input.element_count;

	if (iteration == 0) {
		for (uint i = 0; i < ELEMENTS_PER_THREAD; i++) {
			uint index = DTid.x * ELEMENTS_PER_THREAD + i;
			if (index >= input.gnb_offset_count)
				break;

			uint node_index = gnb_offsets[index].offset;
			float4x4 node_transform = gnb_offsets[index].transform;
			Bounds bb = gnb[node_index].bounds;

			float3 vertices[8] = {
				float3(bb.pMin.x, bb.pMin.y, bb.pMin.z),
				float3(bb.pMin.x, bb.pMin.y, bb.pMax.z),
				float3(bb.pMin.x, bb.pMax.y, bb.pMin.z),
				float3(bb.pMin.x, bb.pMax.y, bb.pMax.z),
				float3(bb.pMax.x, bb.pMin.y, bb.pMin.z),
				float3(bb.pMax.x, bb.pMin.y, bb.pMax.z),
				float3(bb.pMax.x, bb.pMax.y, bb.pMin.z),
				float3(bb.pMax.x, bb.pMax.y, bb.pMax.z),
			};

			float3 npMin, npMax;
			for (uint d = 0; d < 8; d++) {
				vertices[d] = mul(node_transform, float4(vertices[d], 1)).xyz;
				if (d == 0) {
					npMin = npMax = vertices[d];
				} else {
					npMin = min(npMin, vertices[d]);
					npMax = max(npMax, vertices[d]);
				}
			}

			bb.pMin = npMin;
			bb.pMax = npMax;


			if (i == 0) {
				pMin = bb.pMin;
				pMax = bb.pMax;
			} else {
				pMin = min(pMin, bb.pMin);
				pMax = max(pMax, bb.pMax);
			}
			//pMin = float3(-100, -100, -100);
			//pMax = float3(100, 100, 100);
		}
	} else {
		for (uint i = 0; i < ELEMENTS_PER_THREAD; i++) {
			uint index = DTid.x * ELEMENTS_PER_THREAD * iteration + i;
			if (index >= element_count)
				break;

			Bounds bounds = bounding_box[index];

			if (i == 0) {
				pMin = bounds.pMin;
				pMax = bounds.pMax;
			} else {
				pMin = min(pMin, bounds.pMin);
				pMax = max(pMax, bounds.pMax);
			}
		}
	}

	Bounds bb;
	bb.pMin = pMin;
	bb.pMax = pMax;

	bounding_box[DTid.x] = bb;
}