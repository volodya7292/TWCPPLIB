#include "pch.h"
#include "TW3DRTAccelerationStructureTL.h"

TW3D::TW3DRTAccelerationStructureTL::TW3DRTAccelerationStructureTL(const TWT::Vector<TW3DVertexMeshInstance*>& VertexMeshInstances) {
	
	/*TWT::UInt64 gvb_size = 0;
	for (TW3DRTAccelerationStructureBL* BLAS : BLASes)
		gvb_size += BLAS->GetVertexBuffersSize();
	gvb = ResourceManager->CreateVertexBuffer(gvb_size);*/
}

TW3D::TW3DRTAccelerationStructureTL::~TW3DRTAccelerationStructureTL() {
}

void TW3D::TW3DRTAccelerationStructureTL::AddBLASInstance(TW3DVertexMeshInstance* BLASInstance) {
	BLAS_instances.push_back(BLASInstance);
}

void TW3D::TW3DRTAccelerationStructureTL::Build() {
}
