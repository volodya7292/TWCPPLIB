#include "pch.h"
#include "TW3DRTAccelerationStructureBL.h"

TW3D::TW3DRTAccelerationStructureBL::TW3DRTAccelerationStructureBL(const TWT::Vector<TW3DResourceVB*>& VertexBuffers) :
	vertex_buffers(move(VertexBuffers)) {

}

TW3D::TW3DRTAccelerationStructureBL::~TW3DRTAccelerationStructureBL() {
}

void TW3D::TW3DRTAccelerationStructureBL::Build() {
}

TWT::UInt64 TW3D::TW3DRTAccelerationStructureBL::GetVertexBuffersSize() {
	TWT::UInt64 size = 0;
	for (TW3DResourceVB* vb : vertex_buffers)
		size += vb->GetSizeInBytes();
	return size;
}
