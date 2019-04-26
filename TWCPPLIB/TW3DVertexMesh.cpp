#include "pch.h"
#include "TW3DVertexMesh.h"

TW3D::TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	TWT::UInt triangle_count = GetTriangleCount();

	morton_codes_buffer = ResourceManager->CreateUnorderedAccessView(triangle_count, sizeof(TWT::UInt));
	morton_indices_buffer = ResourceManager->CreateUnorderedAccessView(triangle_count, sizeof(TWT::UInt));
	bounding_box_buffer = ResourceManager->CreateUnorderedAccessView(triangle_count, sizeof(TWT::Bounds));
	lbvh_node_buffer = ResourceManager->CreateUnorderedAccessView(GetNodeCount(), sizeof(TWT::LBVHNode));
	lbvh_node_lock_buffer = ResourceManager->CreateUnorderedAccessView(triangle_count, sizeof(TWT::Int));

	ResourceManager->ResourceBarrier(lbvh_node_lock_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

TW3D::TW3DVertexMesh::~TW3DVertexMesh() {
	delete bounding_box_buffer;
	delete morton_codes_buffer;
	delete morton_indices_buffer;
	delete lbvh_node_buffer;
	delete lbvh_node_lock_buffer;
}

TW3D::TW3DResourceUAV* TW3D::TW3DVertexMesh::GetBBBufferResource() {
	return bounding_box_buffer;
}

TW3D::TW3DResourceUAV* TW3D::TW3DVertexMesh::GetMCBufferResource() {
	return morton_codes_buffer;
}

TW3D::TW3DResourceUAV* TW3D::TW3DVertexMesh::GetMCIBufferResource() {
	return morton_indices_buffer;
}

TW3D::TW3DResourceUAV* TW3D::TW3DVertexMesh::GetLBVHNodeBufferResource() {
	return lbvh_node_buffer;
}

TW3D::TW3DResourceUAV* TW3D::TW3DVertexMesh::GetLBVHNodeLockBufferResource() {
	return lbvh_node_lock_buffer;
}

TWT::UInt TW3D::TW3DVertexMesh::GetGVBVertexOffset() {
	return VertexBuffers[0]->GetVertexOffset();
}

TWT::UInt TW3D::TW3DVertexMesh::GetGNBOffset() {
	return GNBNodeOffset;
}

void TW3D::TW3DVertexMesh::SetGNBOffset(TWT::UInt NodeOffset) {
	GNBNodeOffset = NodeOffset;
}

TWT::UInt TW3D::TW3DVertexMesh::GetNodeCount() {
	return 2 * GetTriangleCount() - 1;
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
