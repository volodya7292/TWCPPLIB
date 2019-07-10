#include "pch.h"
#include "TW3DResource.h"

TW3DResource::TW3DResource(TWT::String Name, TW3DDevice* Device, CD3DX12_HEAP_PROPERTIES const& HeapProperties, TW3DTempGCL* TempGCL,
	D3D12_RESOURCE_STATES InitialState, bool UpdateOptimized, D3D12_HEAP_FLAGS HeapFlags, D3D12_CLEAR_VALUE const& ClearValue) :
	name(Name), device(Device), temp_gcl(TempGCL), heap_properties(HeapProperties), heap_flags(HeapFlags),
	InitialState(InitialState), clear_value(ClearValue), update_optimized(UpdateOptimized) {

}

TW3DResource::TW3DResource(TWT::String Name, ID3D12Resource* Resource) :
	name(Name), Native(Resource), update_optimized(false) {
}

TW3DResource::~TW3DResource() {
	Release();
}

D3D12_RESOURCE_DESC TW3DResource::GetDescription() {
	return desc;
}

D3D12_CLEAR_VALUE TW3DResource::GetClearValue() {
	return clear_value;
}

D3D12_GPU_VIRTUAL_ADDRESS TW3DResource::GetGPUVirtualAddress() {
	return Native->GetGPUVirtualAddress();
}

TWT::String const& TW3DResource::GetName() {
	return name;
}

void TW3DResource::Release() {
	if (Native)
		TWU::DXSafeRelease(Native);

	delete staging;
}

void TW3DResource::Create() {
	if (clear_value.Format == DXGI_FORMAT_UNKNOWN)
		device->CreateCommittedResource(&heap_properties, heap_flags, &desc, InitialState, &Native);
	else
		device->CreateCommittedResource(&heap_properties, heap_flags, &desc, InitialState, &Native, &clear_value);

	if (update_optimized)
		AllocateStaging();

	device->MakeResident(Native);
}

void TW3DResource::Create(D3D12_RESOURCE_DESC const& ResourceDescription) {
	desc = ResourceDescription;
	Create();
}

void TW3DResource::Map(TWT::uint SubResourceIndex, const D3D12_RANGE* ReadRange, void** Data) {
	TWU::SuccessAssert(Native->Map(SubResourceIndex, ReadRange, Data), "TW3DResource::Map"s);
}

void TW3DResource::Unmap(TWT::uint SubResourceIndex, const D3D12_RANGE* WrittenRange) {
	Native->Unmap(SubResourceIndex, WrittenRange);
}

void TW3DResource::Read(void* Out, TWT::uint ByteOffset, TWT::uint ByteCount) const {
	ID3D12Resource* read_heap;
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteCount),
		D3D12_RESOURCE_STATE_COPY_DEST,
		&read_heap);
	Native->SetName(L"TW3DResource UAV Read Buffer Heap");

	temp_gcl->Reset();
	temp_gcl->Get()->Native->CopyBufferRegion(read_heap, 0, Native, ByteOffset, ByteCount);
	temp_gcl->Execute();

	D3D12_RANGE range = CD3DX12_RANGE(0, ByteCount);
	void* data;

	read_heap->Map(0, &range, &data);
	memcpy(Out, data, ByteCount);
	read_heap->Unmap(0, nullptr);

	TWU::DXSafeRelease(read_heap);
}

void TW3DResource::AllocateStaging() {
	if (!staging) {
		staging = new TW3DResource(name + "-staging"s, device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), nullptr, D3D12_RESOURCE_STATE_GENERIC_READ);
		staging->Create(CD3DX12_RESOURCE_DESC::Buffer(device->GetResourceByteSize(&desc, 1)));
		staging->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&staging_addr));
	}
}

void TW3DResource::DeallocateStaging() {
	if (staging && !update_optimized) {
		delete staging;
		staging = nullptr;
	}
}


D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	return CD3DX12_RESOURCE_BARRIER::Transition(Resource->Native, StateBefore, StateAfter);
}
