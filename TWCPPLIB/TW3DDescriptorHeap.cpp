#include "pch.h"
#include "TW3DDescriptorHeap.h"

TW3D::TW3DDescriptorHeap::TW3DDescriptorHeap(TW3DDevice* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, TWT::UInt Count) :
	IncrementSize(Device->GetDescriptorHandleIncrementSize(Type))
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = Type;
	desc.NumDescriptors = Count;
	desc.Flags = Flags;

	Device->CreateDescriptorHeap(&desc, &descriptor_heap);
}

TW3D::TW3DDescriptorHeap::~TW3DDescriptorHeap() {
	TWU::DXSafeRelease(descriptor_heap);
}

ID3D12DescriptorHeap* TW3D::TW3DDescriptorHeap::Get() {
	return descriptor_heap;
}

TWT::Int TW3D::TW3DDescriptorHeap::Allocate() {
	return descriptor_count++;
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetCPUDescriptorHandleForHeapStart() {
	return descriptor_heap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetGPUDescriptorHandleForHeapStart() {
	return descriptor_heap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetCPUHandle(TWT::Int Index) {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(GetCPUDescriptorHandleForHeapStart(), Index, IncrementSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetGPUHandle(TWT::Int Index) {
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(GetGPUDescriptorHandleForHeapStart(), Index, IncrementSize);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForRTV(TW3DDevice* Device, TWT::UInt Count) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, Count);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForDSV(TW3DDevice* Device, TWT::UInt Count) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, Count);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForSR(TW3DDevice* Device, TWT::UInt Count) {
	return new TW3DDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, Count);
}
