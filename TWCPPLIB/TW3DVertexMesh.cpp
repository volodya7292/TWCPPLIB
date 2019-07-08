#include "pch.h"
#include "TW3DVertexMesh.h"

TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const std::vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	changed = true;
}

TW3DVertexMesh::~TW3DVertexMesh() {

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
