#pragma once
#include "TW3DRTAccelerationStructureBL.h"
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"

namespace TW3D {
	struct TW3DRTAccelerationStructureBLInstance
	{
		TW3DRTAccelerationStructureBL* BLAS;
		TW3DTransform Transform;
	};

	class TW3DRTAccelerationStructureTL
	{
	public:
		TW3DRTAccelerationStructureTL(const TWT::Vector<TW3DRTAccelerationStructureBL*>& BLASes,
			const TWT::Vector<TW3DRTAccelerationStructureBLInstance*>& BLASInstances, TW3DResourceManager* ResourceManager);
		~TW3DRTAccelerationStructureTL();

		void Build();

	private:
		TW3DResourceVB* gvb;
	};
}