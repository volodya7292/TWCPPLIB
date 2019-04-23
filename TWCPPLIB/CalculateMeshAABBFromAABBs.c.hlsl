#include "HLSLHelper.hlsli"

#define ElementsSummedPerThread 8

struct SceneAABBCalculatorConstants {
	uint NumberOfElements;
};

RWStructuredBuffer<Bounds> AABBBuffer : UAV_REGISTER(SceneAABBCalculatorAABBBuffer);
RWStructuredBuffer<Bounds> OutputBuffer : UAV_REGISTER(SceneAABBCalculatorOutputBuffer);
cbuffer SceneAABBCalculatorConstants : CONSTANT_REGISTER(SceneAABBCalculatorConstantsRegister) {
	SceneAABBCalculatorConstants Constants;
}

AABB CalculateSceneAABB(uint baseElementIndex) {
	uint aabbsToRead = min(Constants.NumberOfElements - baseElementIndex, ElementsSummedPerThread);

	AABB sceneAABB;
	sceneAABB.min = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	sceneAABB.max = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (uint i = 0; i < aabbsToRead; i++) {
		AABB aabb = AABBBuffer[baseElementIndex + i];
		sceneAABB.min = min(aabb.min, sceneAABB.min);
		sceneAABB.max = max(aabb.max, sceneAABB.max);
	}
	return sceneAABB;
}

[numthreads(THREAD_GROUP_1D_WIDTH, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint outputAABBIndex = DTid.x;
	uint BaseElementIndex = DTid.x * ElementsSummedPerThread;
	if (BaseElementIndex >= Constants.NumberOfElements) return;

	OutputBuffer[outputAABBIndex] = CalculateSceneAABB(BaseElementIndex);
}