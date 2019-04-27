#include "pch.h"
#include "TW3DVertexMesh.h"

TW3D::TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	LBVH = new TW3DLBVH(ResourceManager);
}

TW3D::TW3DVertexMesh::~TW3DVertexMesh() {
	delete LBVH;
}

TWT::UInt TW3D::TW3DVertexMesh::GetVertexCount() {
	TWT::UInt64 count = 0;
	for (TW3DVertexBuffer* vb : VertexBuffers)
		count += vb->GetVertexCount();
	return count;
}

TWT::UInt TW3D::TW3DVertexMesh::GetTriangleCount() {
	return GetVertexCount() / 3;
}
