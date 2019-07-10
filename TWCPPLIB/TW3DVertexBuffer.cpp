#include "pch.h"
#include "TW3DVertexBuffer.h"

TW3DVertexBuffer::TW3DVertexBuffer(TWT::String Name, TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint VertexCount, TWT::uint SingleVertexSize) :
	TW3DResource(Name, Device,
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), TempGCL,
	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, OptimizeForUpdating),
	vertex_count(VertexCount), single_vertex_size(SingleVertexSize) {

	TWT::uint64 size = vertex_count * single_vertex_size;

	desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	TW3DResource::Create();
	Native->SetName(L"TW3DResourceVB");

	view.BufferLocation = Native->GetGPUVirtualAddress();
	view.SizeInBytes    = size;
	view.StrideInBytes  = SingleVertexSize;
}

D3D12_VERTEX_BUFFER_VIEW TW3DVertexBuffer::GetView() {
	return view;
}

void TW3DVertexBuffer::Update(const void* Data, TWT::uint VertexCount) {
	vertex_count = VertexCount;
	TWT::uint64 size = vertex_count * single_vertex_size;

	AllocateStaging();

	memcpy(staging_addr, Data, size);

	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(Native, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->CopyBufferRegion(this, 0, staging, 0, size);
	temp_gcl->ResourceBarrier(Native, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	temp_gcl->Execute();

	DeallocateStaging();
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