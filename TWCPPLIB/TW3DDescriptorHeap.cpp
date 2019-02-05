#include "pch.h"
#include "TW3DDescriptorHeap.h"

TW3D::TW3DDescriptorHeap::TW3DDescriptorHeap(TW3DDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, TWT::UInt count) {
	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.Type = type;
	desc.NumDescriptors = count;
	desc.Flags = flags;

	device->CreateDescriptorHeap(&desc, &descriptorHeap);
}

TW3D::TW3DDescriptorHeap::~TW3DDescriptorHeap() {
	TWU::DXSafeRelease(descriptorHeap);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::createForRTV(TW3DDevice* device, TWT::UInt count) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, count);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::createForDSV(TW3DDevice* device) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DDescriptorHeap::createForSR(TW3DDevice* device, TWT::UInt count) {
	return new TW3DDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, count);
}
