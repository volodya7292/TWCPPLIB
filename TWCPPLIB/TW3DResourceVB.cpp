#include "pch.h"
#include "TW3DResourceVB.h"

TW3DResourceVB::TW3DResourceVB(TW3DDevice* Device, TWT::uint VertexCount, TWT::uint SingleVertexSize, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), vertex_count(VertexCount), single_vertex_size(SingleVertexSize), temp_gcl(TempGCL) {

	TWT::uint64 size = vertex_count * single_vertex_size;

	upload_heap = new TW3DResource(Device,
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&Resource);
	Resource->SetName(L"TW3DResourceVB");

	upload_heap->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpu_address));

	view.BufferLocation = Resource->GetGPUVirtualAddress();
	view.SizeInBytes    = size;
	view.StrideInBytes  = SingleVertexSize;
}

TW3DResourceVB::~TW3DResourceVB() {
	delete upload_heap;
}

D3D12_VERTEX_BUFFER_VIEW TW3DResourceVB::GetView() {
	return view;
}

void TW3DResourceVB::UpdateData(const void* Data, TWT::uint VertexCount) {
	vertex_count = VertexCount;
	TWT::uint64 size = vertex_count * single_vertex_size;

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData		= Data;
	vertexData.RowPitch		= size;
	vertexData.SlicePitch	= size;

	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(Resource, upload_heap->Get(), &vertexData);
	temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	temp_gcl->Execute();
}

TWT::uint TW3DResourceVB::GetVertexCount() {
	return vertex_count;
}

TWT::uint TW3DResourceVB::GetVertexByteSize() {
	return single_vertex_size;
}

TWT::uint TW3DResourceVB::GetSizeInBytes() {
	return vertex_count * single_vertex_size;
}