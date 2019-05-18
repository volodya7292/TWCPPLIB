#include "pch.h"
#include "TW3DVertexMesh.h"

TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	lbvh = new TW3DLBVH(ResourceManager, GetTriangleCount());
	changed = true;
}

TW3DVertexMesh::~TW3DVertexMesh() {
	delete lbvh;
}

TWT::uint TW3DVertexMesh::GetVertexCount() {
	TWT::uint64 count = 0;
	for (TW3DVertexBuffer* vb : VertexBuffers)
		count += vb->GetVertexCount();
	return count;
}

TWT::uint TW3DVertexMesh::GetTriangleCount() {
	return GetVertexCount() / 3;
}

TW3DLBVH* TW3DVertexMesh::GetLBVH() {
	return lbvh;
}

void TW3DVertexMesh::UpdateLBVH(TW3DResourceUAV* GVB, TWT::uint GVBOffset, TW3DGraphicsCommandList* CommandList) {
	if (changed) {
		changed = false;
		lbvh->BuildFromTriangles(GVB, GVBOffset, CommandList);
	}
}
