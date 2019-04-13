#include "pch.h"
#include "TW3DRTAccelerationStructureTL.h"

TW3D::TW3DRTAccelerationStructureTL::TW3DRTAccelerationStructureTL(const TWT::Vector<TW3DRTAccelerationStructureBL*>& BLASes,
	const TWT::Vector<TW3DRTAccelerationStructureBLInstance*>& BLASInstances, TW3DResourceManager* ResourceManager) {
	
	TWT::UInt64 gvb_size = 0;
	for (TW3DRTAccelerationStructureBL* BLAS : BLASes)
		gvb_size += BLAS->GetVertexBuffersSize();
	gvb = ResourceManager->CreateVertexBuffer(gvb_size);
}

TW3D::TW3DRTAccelerationStructureTL::~TW3DRTAccelerationStructureTL() {
}

void TW3D::TW3DRTAccelerationStructureTL::Build() {
}
