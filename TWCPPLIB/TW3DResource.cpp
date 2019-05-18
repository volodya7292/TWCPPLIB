#include "pch.h"
#include "TW3DResource.h"

TW3DResource::TW3DResource(TW3DDevice* Device, const CD3DX12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags,
	const D3D12_RESOURCE_DESC* ResourceDescription, D3D12_RESOURCE_STATES ResourceStates, const D3D12_CLEAR_VALUE* OptimizedClearValue) :
	Device(Device)
{
	Device->CreateCommittedResource(HeapProperties, HeapFlags, ResourceDescription, ResourceStates, &Resource, OptimizedClearValue);
	Resource->SetName(L"TW3DResource");
}

TW3DResource::TW3DResource(ID3D12Resource* resource) : 
	Resource(resource)
{
}

TW3DResource::TW3DResource(TW3DDevice* Device) :
	Device(Device)
{	
}

TW3DResource::~TW3DResource() {
	Release();
}

ID3D12Resource* TW3DResource::Get() {
	return Resource;
}

D3D12_GPU_VIRTUAL_ADDRESS TW3DResource::GetGPUVirtualAddress() {
	return Resource->GetGPUVirtualAddress();
}

void TW3DResource::Release() {
	TWU::DXSafeRelease(Resource);
}

void TW3DResource::Map(TWT::uint SubResourceIndex, D3D12_RANGE* ReadRange, void** Data) {
	TWU::SuccessAssert(Resource->Map(SubResourceIndex, ReadRange, Data), "TW3DResource::Map"s);
}

TW3DResource* TW3DResource::Create(TW3DDevice* Device, TWT::uint64 Size, bool Staging) {
	return new TW3DResource(Device,
		&CD3DX12_HEAP_PROPERTIES(Staging ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		Staging ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST
	);
}

TW3DResource* TW3DResource::CreateCBStaging(TW3DDevice* Device) {
	return Create(Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, true);
}

D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	return CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter);
}
