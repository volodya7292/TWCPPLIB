#include "pch.h"
#include "TW3DBuffer.h"

TW3DBuffer::TW3DBuffer(TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint ElementSizeInBytes, bool UAV, TW3DDescriptorHeap* SRVDescriptorHeap) :
	TW3DResource(Device, TempGCL,
	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, OptimizeForUpdating),
	srv_descriptor_heap(SRVDescriptorHeap), element_size(ElementSizeInBytes)
{
	srv_index = SRVDescriptorHeap->Allocate();
	if (UAV)
		uav_index = SRVDescriptorHeap->Allocate();

	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
}

TW3DBuffer::~TW3DBuffer() {
	srv_descriptor_heap->Free(srv_index);
	srv_descriptor_heap->Free(uav_index);
}

TWT::uint TW3DBuffer::GetElementCount() {
	return element_count;
}

void TW3DBuffer::Create(TWT::uint ElementCount) {
	element_count = ElementCount;

	srv_desc.Buffer.NumElements = ElementCount;
	uav_desc.Buffer.NumElements = ElementCount;

	desc = CD3DX12_RESOURCE_DESC::Buffer(ElementCount * element_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	TW3DResource::Create();
	resource->SetName(L"TW3DResourceUAV Buffer");

	device->CreateShaderResourceView(resource, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, srv_descriptor_heap->GetCPUHandle(uav_index));
}

void TW3DBuffer::Update(const void* Data, TWT::uint ElementCount) {
	element_count = ElementCount;

	TWT::uint64 size = ElementCount * element_size;

	D3D12_SUBRESOURCE_DATA upload_data = {};
	upload_data.pData = Data;
	upload_data.RowPitch = size;
	upload_data.SlicePitch = size;

	TW3DResource* upload_heap = staging;
	if (!staging)
		upload_heap = TW3DResource::CreateStaging(device, size);

	temp_gcl->Reset();
	//temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(resource, upload_heap->Get(), &upload_data);
	//temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	temp_gcl->Execute();

	if (!staging)
		delete upload_heap;
}
