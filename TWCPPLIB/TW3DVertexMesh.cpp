#include "pch.h"
#include "TW3DVertexMesh.h"

TW3D::TW3DVertexMesh::TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers) :
	VertexBuffers(move(VertexBuffers))
{
	constant_buffer = ResourceManager->CreateConstantBuffer(1, sizeof(TWT::DefaultVertexMeshCB));
	morton_codes_buffer = ResourceManager->CreateUnorderedAccessView(GetVertexCount() / 3, sizeof(TWT::UInt));
	morton_indices_buffer = ResourceManager->CreateUnorderedAccessView(GetVertexCount() / 3, sizeof(TWT::UInt));
	bounding_box_buffer = ResourceManager->CreateUnorderedAccessView(GetVertexCount() / 3, sizeof(TWT::Bounds));

	TWT::DefaultVertexMeshCB cb;
	cb.vertex_info.x = VertexBuffers[0]->GetVertexOffset();
	cb.vertex_info.y = GetVertexCount();

	constant_buffer->Update(&cb, 0);
}

TW3D::TW3DVertexMesh::~TW3DVertexMesh() {
	delete constant_buffer;
	delete bounding_box_buffer;
	delete morton_codes_buffer;
	delete morton_indices_buffer;
}

TW3D::TW3DResourceCB* TW3D::TW3DVertexMesh::GetCBResource() {
	return constant_buffer;
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

TWT::UInt TW3D::TW3DVertexMesh::GetVertexCount() {
	TWT::UInt64 count = 0;
	for (TW3DVertexBuffer* vb : VertexBuffers)
		count += vb->GetVertexCount();
	return count;
}

TWT::UInt TW3D::TW3DVertexMesh::GetTriangleCount() {
	return GetVertexCount() / 3;
}
