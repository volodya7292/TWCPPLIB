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

	TW3DComputePipeline* GetComputeShader(TWT::uint Id);
	TW3DGraphicsPipeline* GetGraphicsShader(TWT::uint Id);
}  // namespace TW3DShaders