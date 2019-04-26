#pragma once
#include "TW3DResourceManager.h"

namespace TW3DShaders {
	enum TW3DComputeShader {
		CalculateMeshBoundingBox,
		CalculateMortonCodes,
		SetupLBVHNodes,
		BuildLBVHSplits,
		UpdateLBVHNodeBounds,
		ComputeShaderCount
	};

	void Initialize(TW3D::TW3DResourceManager* ResourceManager);
	void Release();

	TW3D::TW3DComputePipelineState* GetComputeShader(TWT::UInt Id);
	TW3D::TW3DGraphicsPipelineState* GetGraphicsShader(TWT::UInt Id);
}