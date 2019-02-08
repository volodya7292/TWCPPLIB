#include "pch.h"
#include "TW3DDescriptorHeap.h"

TW3D::TW3DDescriptorHeap::TW3DDescriptorHeap(TW3DDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, TWT::UInt count) {
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = count;
	desc.Flags = flags;

	device->CreateDescriptorHeap(&desc, &descriptorHeap);
}

TW3D::TW3DDescriptorHeap::~TW3DDescriptorHeap() {
	TWU::DXSafeRelease(descriptorHeap);
}

ID3D12DescriptorHeap* TW3D::TW3DDescriptorHeap::Get() {
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetCPUDescriptorHandleForHeapStart() {
	return descriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetGPUDescriptorHandleForHeapStart() {
	return descriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DDescriptorHeap::GetHandle(TWT::Int Offset, TWT::UInt IncrementSize) {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(GetCPUDescriptorHandleForHeapStart(), Offset, IncrementSize);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForRTV(TW3DDevice* device, TWT::UInt count) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, count);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForDSV(TW3DDevice* device) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::CreateForSR(TW3DDevice* device, TWT::UInt count) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, count);
}
