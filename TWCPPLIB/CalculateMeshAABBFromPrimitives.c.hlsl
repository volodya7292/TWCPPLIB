#define HLSL
#include "HLSLHelper.hlsli"

#define ElementsSummedPerThread 8

struct SceneAABBCalculatorConstants {
	uint NumberOfElements;
};

RWStructuredBuffer<AABB> AABBBuffer : UAV_REGISTER(SceneAABBCalculatorAABBBuffer);
RWStructuredBuffer<AABB> OutputBuffer : UAV_REGISTER(SceneAABBCalculatorOutputBuffer);
cbuffer SceneAABBCalculatorConstants : CONSTANT_REGISTER(SceneAABBCalculatorConstantsRegister) {
	SceneAABBCalculatorConstants Constants;
}

AABB CalculateSceneAABB(uint baseElementIndex);

RWStructuredBuffer<Primitive> InputBuffer : UAV_REGISTER(SceneAABBCalculatorInputBufferRegister);

AABB CalculateSceneAABB(uint baseElementIndex) {
	uint primitivesToRead = min(Constants.NumberOfElements - baseElementIndex, ElementsSummedPerThread);

	AABB sceneAABB;
	sceneAABB.min = float3(FLT_MAX, FLT_MAX, FLT_MAX);
	sceneAABB.max = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (uint i = 0; i < primitivesToRead; i++) {
		Primitive primitive = InputBuffer[baseElementIndex + i];
		if (primitive.PrimitiveType == TRIANGLE_TYPE) {
			Triangle tri = GetTriangle(primitive);
			sceneAABB.min = min(min(min(tri.v0, sceneAABB.min), tri.v1), tri.v2);
			sceneAABB.max = max(max(max(tri.v0, sceneAABB.max), tri.v1), tri.v2);
		} else // if(primitive.PrimitiveType == PROCEDURAL_PRIMITIVE_TYPE)
		{
			AABB aabb = GetProceduralPrimitiveAABB(primitive);
			sceneAABB.min = min(sceneAABB.min, aabb.min);
			sceneAABB.max = max(sceneAABB.max, aabb.max);
		}
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