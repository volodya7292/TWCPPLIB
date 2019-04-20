#include "pch.h"
#include "TW3DVertexMesh.h"

TW3D::TW3DVertexMesh::TW3DVertexMesh(const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers)) {

}

TW3D::TW3DVertexMesh::~TW3DVertexMesh() {
}

TWT::UInt64 TW3D::TW3DVertexMesh::GetVertexBuffersSize() {
	TWT::UInt64 size = 0;
	for (TW3DVertexBuffer* vb : VertexBuffers)
		size += vb->GetSizeInBytes();
	return size;
}
