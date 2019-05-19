#include "pch.h"
#include "TW3DVertexBuffer.h"

TW3DVertexBuffer::TW3DVertexBuffer(TW3DDevice* Device, TW3DTempGCL* TempGCL, TWT::uint VertexCount, TWT::uint SingleVertexSize) :
	TW3DResource(Device, TempGCL), vertex_count(VertexCount), single_vertex_size(SingleVertexSize) {

	TWT::uint64 size = vertex_count * single_vertex_size;

	upload_heap = TW3DResource::CreateStaging(Device, size);

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&resource);
	resource->SetName(L"TW3DResourceVB");

	upload_heap->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpu_address));

	view.BufferLocation = resource->GetGPUVirtualAddress();
	view.SizeInBytes    = size;
	view.StrideInBytes  = SingleVertexSize;
}

TW3DVertexBuffer::~TW3DVertexBuffer() {
	delete upload_heap;
}

D3D12_VERTEX_BUFFER_VIEW TW3DVertexBuffer::GetView() {
	return view;
}

void TW3DVertexBuffer::Update(const void* Data, TWT::uint VertexCount) {
	vertex_count = VertexCount;
	TWT::uint64 size = vertex_count * single_vertex_size;

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData		= Data;
	vertexData.RowPitch		= size;
	vertexData.SlicePitch	= size;

	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(resource, upload_heap->Get(), &vertexData);
	temp_gcl->ResourceBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	temp_gcl->Execute();
}

TWT::uint TW3DVertexBuffer::GetVertexCount() {
	return vertex_count;
}

TWT::uint TW3DVertexBuffer::GetVertexByteSize() {
	return single_vertex_size;
}

TWT::uint TW3DVertexBuffer::GetSizeInBytes() {
	return vertex_count * single_vertex_size;
}