#include "pch.h"
#include "TW3DResourceUAV.h"
#include "TW3DTempGCL.h"

TW3DResourceUAV::TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::UInt ElementSizeInBytes, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap), element_size(ElementSizeInBytes), Format(DXGI_FORMAT_UNKNOWN), temp_gcl(TempGCL) {
	SRVIndex = SRVDescriptorHeap->Allocate(); // For SRV
	UAVIndex = SRVDescriptorHeap->Allocate(); // For UAV

	srv_desc = {};
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	uav_desc = {};
	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
}

TW3DResourceUAV::TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap), Format(Format) {
	SRVIndex = SRVDescriptorHeap->Allocate(); // For SRV
	UAVIndex = SRVDescriptorHeap->Allocate(); // For UAV
}

TW3DResourceUAV::~TW3DResourceUAV() {
	SRVDescriptorHeap->Free(SRVIndex);
	SRVDescriptorHeap->Free(UAVIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DResourceUAV::GetGPUSRVHandle() {
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DResourceUAV::GetCPUUAVHandle() {
	return SRVDescriptorHeap->GetCPUHandle(UAVIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DResourceUAV::GetGPUUAVHandle() {
	return SRVDescriptorHeap->GetGPUHandle(UAVIndex);
}

TWT::UInt TW3DResourceUAV::GetElementCount() {
	return element_count;
}

void TW3DResourceUAV::UpdateData(const void* Data, TWT::UInt ElementCount) {
	element_count = ElementCount;

	TWT::UInt64 size = ElementCount * element_size;

	D3D12_SUBRESOURCE_DATA upload_data = {};
	upload_data.pData = Data;
	upload_data.RowPitch = size;
	upload_data.SlicePitch = size;

	ID3D12Resource* upload_heap;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&upload_heap);
	Resource->SetName(L"TW3DResource Upload Buffer Heap");

	temp_gcl->Reset();
	//temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(Resource, upload_heap, &upload_data);
	//temp_gcl->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	temp_gcl->Execute();

	TWU::DXSafeRelease(upload_heap);
}

void TW3DResourceUAV::Read(void* Out, TWT::UInt ByteOffset, TWT::UInt ByteCount) {
	ID3D12Resource* read_heap;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteCount),
		D3D12_RESOURCE_STATE_COPY_DEST,
		&read_heap);
	Resource->SetName(L"TW3DResource UAV Read Buffer Heap");

	temp_gcl->Reset();
	temp_gcl->Get()->Get()->CopyBufferRegion(read_heap, 0, Resource, ByteOffset, ByteCount);
	temp_gcl->Execute();

	D3D12_RANGE range = CD3DX12_RANGE(0, ByteCount);
	void* data;
	read_heap->Map(0, &range, &data);

	memcpy(Out, data, ByteCount);

	read_heap->Unmap(0, nullptr);

	TWU::DXSafeRelease(read_heap);
}

void TW3DResourceUAV::CreateBuffer(TWT::UInt ElementCount) {
	element_count = ElementCount;

	srv_desc.Buffer.NumElements = ElementCount;
	uav_desc.Buffer.NumElements = ElementCount;

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ElementCount * element_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&Resource);
	Resource->SetName(L"TW3DResourceUAV Buffer");

	Device->CreateShaderResourceView(Resource, &srv_desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
	Device->CreateUnorderedAccessView(Resource, &uav_desc, SRVDescriptorHeap->GetCPUHandle(UAVIndex));
}

void TW3DResourceUAV::CreateTexture2D(TWT::UInt Width, TWT::UInt Height) {
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(Format, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&Resource);
	Resource->SetName(L"TW3DResourceUAV 2D");

	Device->CreateShaderResourceView(Resource, nullptr, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
	Device->CreateUnorderedAccessView(Resource, nullptr, SRVDescriptorHeap->GetCPUHandle(UAVIndex));
}

D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = Resource ? Resource->Get() : nullptr;
	return barrier;
}
