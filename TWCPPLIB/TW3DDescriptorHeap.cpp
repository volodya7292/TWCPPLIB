#include "pch.h"
#include "TW3DDescriptorHeap.h"

TW3DDescriptorHeap::TW3DDescriptorHeap(TW3DDevice* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, TWT::uint DescriptorCount) :
	IncrementSize(Device->GetDescriptorHandleIncrementSize(Type))
{
	memset(free_descriptors, 1, sizeof(free_descriptors));

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = Type;
	desc.NumDescriptors = DescriptorCount;
	desc.Flags = Flags;

	Device->CreateDescriptorHeap(&desc, &descriptor_heap);
	Device->MakeResident(descriptor_heap);

	descriptor_heap->SetName(L"TW3DDescriptorHeap");
}

TW3DDescriptorHeap::~TW3DDescriptorHeap() {
	TWU::DXSafeRelease(descriptor_heap);
}

ID3D12DescriptorHeap* TW3DDescriptorHeap::Get() {
	return descriptor_heap;
}

int TW3DDescriptorHeap::Allocate() {
	for (size_t i = 0; i < MAX_DESCRIPTOR_COUNT; i++)
		if (free_descriptors[i]) {
			free_descriptors[i] = false;
			return i;
		}
	throw std::runtime_error("NO FREE DESCRIPTORS");
}

void TW3DDescriptorHeap::Free(int DescriptorIndex) {
	if (DescriptorIndex >= 0)
		free_descriptors[DescriptorIndex] = true;
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DDescriptorHeap::GetCPUDescriptorHandleForHeapStart() {
	return descriptor_heap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DDescriptorHeap::GetGPUDescriptorHandleForHeapStart() {
	return descriptor_heap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DDescriptorHeap::GetCPUHandle(int Index) {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(GetCPUDescriptorHandleForHeapStart(), Index, IncrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DDescriptorHeap::GetGPUHandle(int Index) {
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(GetGPUDescriptorHandleForHeapStart(), Index, IncrementSize);
}

TW3DDescriptorHeap* TW3DDescriptorHeap::CreateForRTV(TW3DDevice* Device, TWT::uint Count, D3D12_DESCRIPTOR_HEAP_FLAGS Flags) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, Flags, Count);
}

TW3DDescriptorHeap* TW3DDescriptorHeap::CreateForDSV(TW3DDevice* Device, TWT::uint Count, D3D12_DESCRIPTOR_HEAP_FLAGS Flags) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, Flags, Count);
}

TW3DDescriptorHeap* TW3DDescriptorHeap::CreateForSR(TW3DDevice* Device, TWT::uint Count, D3D12_DESCRIPTOR_HEAP_FLAGS Flags) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Flags, Count);
}
