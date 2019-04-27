#pragma once
#include "TW3DResourceManager.h"

namespace TW3DShaders {
	enum ComputeShader {
		CalculateMeshBoundingBox,
		CalculateMortonCodes,
		SetupLBVHNodes,
		BuildLBVHSplits,
		UpdateLBVHNodeBounds,
		BuildGLBVHNB,
		CalculateLBVHsBoundingBox,
		CalculateMortonCodesForLBVHs,
		ComputeShaderCount
	};

	enum GraphicsShader {
		BuildGVB,
		GraphicsShaderCount
	};

	void Initialize(TW3D::TW3DResourceManager* ResourceManager);
	void Release();

	TW3D::TW3DComputePipelineState* GetComputeShader(TWT::UInt Id);
	TW3D::TW3DGraphicsPipelineState* GetGraphicsShader(TWT::UInt Id);
}