#pragma once
#include "TW3DResourceManager.h"

namespace TW3DShaders {
	enum ComputeShader {
		CalculateTriangleMeshBoundingBox,
		CalculateMortonCodesForTriangles,
		SetupLBVHNodesFromTriangles,
		BuildLBVHSplits,
		UpdateLBVHNodeBounds,
		CalculateLBVHsBoundingBox,
		CalculateMortonCodesForLBVHs,
		SetupLBVHNodesFromLBVHs,
		BuildLBVHSplitsFromLBVHs,
		UpdateLBVHNodeBoundsForLBVHs,
		ComputeShaderCount
	};

	enum GraphicsShader {
		GraphicsShaderCount
	};

	void Initialize(TW3D::TW3DResourceManager* ResourceManager);
	void Release();

	TW3D::TW3DComputePipelineState* GetComputeShader(TWT::UInt Id);
	TW3D::TW3DGraphicsPipelineState* GetGraphicsShader(TWT::UInt Id);
}