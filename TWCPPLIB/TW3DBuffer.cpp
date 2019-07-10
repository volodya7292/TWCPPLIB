#include "pch.h"
#include "TW3DBuffer.h"

TW3DBuffer::TW3DBuffer(TWT::String Name, TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint ElementSizeInBytes, bool UAV, TW3DDescriptorHeap* SRVDescriptorHeap) :
	TW3DResource(Name, Device,
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), TempGCL,
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

const TWT::uint TW3DBuffer::GetElementCount() const{
	return element_count;
}

const TWT::uint TW3DBuffer::GetMaxElementCount() const {
	return max_element_count;
}

void TW3DBuffer::Create(TWT::uint MaxElementCount) {
	max_element_count = MaxElementCount;

	srv_desc.Buffer.NumElements = max_element_count;
	uav_desc.Buffer.NumElements = max_element_count;

	desc = CD3DX12_RESOURCE_DESC::Buffer(max_element_count * element_size, ResourceFlags | (uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
	TW3DResource::Create();
	Native->SetName(L"TW3DResourceUAV Buffer");

	device->CreateShaderResourceView(Native, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(Native, &uav_desc, srv_descriptor_heap->GetCPUHandle(uav_index));
}

void TW3DBuffer::Update(const void* Data, TWT::uint ElementCount) {
	if (ElementCount > max_element_count)
		TWU::TW3DLogError("TW3DBuffer::Update \'"s + name + "\' ElementCount > MaxElementCount !"s);

	element_count = ElementCount;

	TWT::uint64 size = ElementCount * element_size;

	D3D12_SUBRESOURCE_DATA upload_data = {};
	upload_data.pData = Data;
	upload_data.RowPitch = size;
	upload_data.SlicePitch = size;

	AllocateStaging();

	memcpy(staging_addr, Data, size);

	temp_gcl->Reset();
	temp_gcl->CopyBufferRegion(this, 0, staging, 0, size);
	temp_gcl->Execute();

	DeallocateStaging();
}

void TW3DBuffer::Update(const void* Data, TWT::uint ElementCount, D3D12_RESOURCE_STATES BeforeUpdate, D3D12_RESOURCE_STATES AfterUpdate) {
	if (ElementCount > max_element_count)
		TWU::TW3DLogError("TW3DBuffer::Update \'"s + name + "\' ElementCount > MaxElementCount !"s);

	element_count = ElementCount;

	TWT::uint64 size = ElementCount * element_size;

	AllocateStaging();

	memcpy(staging_addr, Data, size);

	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(Native, AfterUpdate, BeforeUpdate);
	temp_gcl->CopyBufferRegion(this, 0, staging, 0, size);
	temp_gcl->ResourceBarrier(Native, BeforeUpdate, AfterUpdate);
	temp_gcl->Execute();

	DeallocateStaging();
}

void TW3DBuffer::UpdateElement(const void* Data, TWT::uint ElementIndex) {
	TWT::uint64 size = element_count * element_size;
	TWT::uint64 index_offset = ElementIndex * element_size;

	AllocateStaging();
	
	memcpy(staging_addr + index_offset, Data, element_size);

	temp_gcl->Reset();
	temp_gcl->CopyBufferRegion(this, index_offset, staging, index_offset, element_size);
	temp_gcl->Execute();

	DeallocateStaging();
}
