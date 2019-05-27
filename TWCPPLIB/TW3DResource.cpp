#include "pch.h"
#include "TW3DResource.h"

TW3DResource::TW3DResource(TW3DDevice* Device, CD3DX12_HEAP_PROPERTIES const& HeapProperties, TW3DTempGCL* TempGCL,
	D3D12_RESOURCE_STATES InitialResourceState, bool OptimizeForUpdating, D3D12_HEAP_FLAGS HeapFlags, D3D12_CLEAR_VALUE const& ClearValue) :
	device(Device), temp_gcl(TempGCL), heap_properties(HeapProperties), heap_flags(HeapFlags), initial_resource_state(InitialResourceState), clear_value(ClearValue)
{
	if (OptimizeForUpdating)
		staging = new TW3DResource(Device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), temp_gcl, D3D12_RESOURCE_STATE_GENERIC_READ, false);
}

TW3DResource::TW3DResource(ID3D12Resource* resource) : 
	resource(resource)
{
}

TW3DResource::~TW3DResource() {
	Release();
}

ID3D12Resource* TW3DResource::Get() {
	return resource;
}

D3D12_CLEAR_VALUE TW3DResource::GetClearValue() {
	return clear_value;
}

D3D12_GPU_VIRTUAL_ADDRESS TW3DResource::GetGPUVirtualAddress() {
	return resource->GetGPUVirtualAddress();
}

void TW3DResource::Release() {
	if (resource)
		TWU::DXSafeRelease(resource);
	
		delete staging;
}

void TW3DResource::Create() {
	if (clear_value.Format == DXGI_FORMAT_UNKNOWN) {
		device->CreateCommittedResource(&heap_properties, heap_flags, &desc, initial_resource_state, &resource);
	} else {
		device->CreateCommittedResource(&heap_properties, heap_flags, &desc, initial_resource_state, &resource, &clear_value);
	}

	if (staging)
		staging->Create(CD3DX12_RESOURCE_DESC::Buffer(device->GetCopyableFootprints(&desc, 1)));
}

void TW3DResource::Create(D3D12_RESOURCE_DESC const& ResourceDescription) {
	desc = ResourceDescription;
	Create();
}

void TW3DResource::Map(TWT::uint SubResourceIndex, D3D12_RANGE* ReadRange, void** Data) {
	TWU::SuccessAssert(resource->Map(SubResourceIndex, ReadRange, Data), "TW3DResource::Map"s);
}

void TW3DResource::Read(void* Out, TWT::uint ByteOffset, TWT::uint ByteCount) {
	ID3D12Resource* read_heap;
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteCount),
		D3D12_RESOURCE_STATE_COPY_DEST,
		&read_heap);
	resource->SetName(L"TW3DResource UAV Read Buffer Heap");

	temp_gcl->Reset();
	temp_gcl->Get()->Get()->CopyBufferRegion(read_heap, 0, resource, ByteOffset, ByteCount);
	temp_gcl->Execute();

	D3D12_RANGE range = CD3DX12_RANGE(0, ByteCount);
	void* data;
	read_heap->Map(0, &range, &data);

	memcpy(Out, data, ByteCount);

	read_heap->Unmap(0, nullptr);

	TWU::DXSafeRelease(read_heap);
}

TW3DResource* TW3DResource::CreateStaging(TW3DDevice* Device, TWT::uint64 Size) {
	TW3DResource* resource = new TW3DResource(Device,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		nullptr,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	resource->Create(CD3DX12_RESOURCE_DESC::Buffer(Size));
	return resource;
}

D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	return CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter);
}
