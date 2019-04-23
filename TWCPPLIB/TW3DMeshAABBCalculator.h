#pragma once
#include "TW3DResourceManager.h"

namespace TW3D {
	class TW3DMeshAABBCalculator {
	public:
		TW3DMeshAABBCalculator(TW3DResourceManager* ResourceManager);
		~TW3DMeshAABBCalculator();

	private:
		enum RootParameterSlot {
			InputBuffer = 0,
			InputConstants,
			InputAABBBuffer,
			OutputBuffer,
			NumParameters
		};

		static UINT GetNumAABBsOutputFromPass(UINT numElements);

		TW3DComputePipelineState* m_pCalculateSceneAABBFromPrimitives;
		TW3DComputePipelineState* m_pCalculateSceneAABBFromAABBs;
	};
}