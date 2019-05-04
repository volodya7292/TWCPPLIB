#include "pch.h"
#include "TW3DVertexMesh.h"

TW3D::TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	lbvh = new TW3DLBVH(ResourceManager, GetTriangleCount());
	changed = true;
}

TW3D::TW3DVertexMesh::~TW3DVertexMesh() {
	delete lbvh;
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

TW3D::TW3DLBVH* TW3D::TW3DVertexMesh::GetLBVH() {
	return lbvh;
}

void TW3D::TW3DVertexMesh::UpdateLBVH(TW3DResourceUAV* GVB, TWT::UInt GVBOffset) {
	if (changed) {
		changed = false;
		lbvh->BuildFromPrimitives(GVB, GVBOffset);
	}
}
