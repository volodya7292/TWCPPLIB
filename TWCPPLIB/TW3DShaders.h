#pragma once
#include "TW3DResourceManager.h"

namespace TW3DShaders {
	enum ComputeShader {
		CalculateTriangleMeshBoundingBox,
		ComputeShaderCount
	};

	enum GraphicsShader {
		GraphicsShaderCount
	};

	void Initialize(TW3DResourceManager* ResourceManager);
	void Release();

	TW3DComputePipelineState* GetComputeShader(TWT::uint Id);
	TW3DGraphicsPipelineState* GetGraphicsShader(TWT::uint Id);
}  // namespace TW3DShaders