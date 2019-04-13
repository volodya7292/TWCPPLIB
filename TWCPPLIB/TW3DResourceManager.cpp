#include "pch.h"
#include "TW3DResourceManager.h"

TW3D::TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
	temp_gcl = new TW3DTempGCL(device);
	rtv_descriptor_heap = TW3DDescriptorHeap::CreateForRTV(device, 1024);
	dsv_descriptor_heap = TW3DDescriptorHeap::CreateForDSV(device, 1024);
	srv_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 1024);
}

TW3D::TW3DResourceManager::~TW3DResourceManager() {
	delete temp_gcl;
	delete rtv_descriptor_heap;
	delete dsv_descriptor_heap;
	delete srv_descriptor_heap;
}

TW3D::TW3DResourceRTV* TW3D::TW3DResourceManager::CreateRenderTargetView(ID3D12Resource* Buffer) {
	TW3DResourceRTV* rtv = new TW3D::TW3DResourceRTV(device, rtv_descriptor_heap);
	rtv->Create(Buffer);
	return rtv;
}

TW3D::TW3DResourceRTV* TW3D::TW3DResourceManager::CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue) {
	TW3DResourceRTV* rtv = new TW3DResourceRTV(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->Create(Width, Height);
	return rtv;
}

TW3D::TW3DResourceDSV* TW3D::TW3DResourceManager::CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height) {
	TW3DResourceDSV* dsv = new TW3DResourceDSV(device, dsv_descriptor_heap);
	dsv->Create(Width, Height);
	return dsv;
}

TW3D::TW3DResourceUAV* TW3D::TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, Format);
	uav->CreateTexture2D(Width, Height);
	return uav;
}

TW3D::TW3DResourceVB* TW3D::TW3DResourceManager::CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes) {
	return new TW3DResourceVB(device, VertexCount * SingleVertexSizeInBytes, SingleVertexSizeInBytes, temp_gcl);
}

TW3D::TW3DResourceCB* TW3D::TW3DResourceManager::CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount) {
	return new TW3DResourceCB(device, ElementSizeInBytes, ElementCount);
}

TW3D::TW3DResourceSR* TW3D::TW3DResourceManager::CreateTexture2D(TWT::WString Filename) {
	return TW3DResourceSR::Create2D(device, srv_descriptor_heap, Filename, temp_gcl);
}

TW3D::TW3DDevice* TW3D::TW3DResourceManager::GetDevice() {
	return device;
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DResourceManager::GetSVDescriptorHeap() {
	return srv_descriptor_heap;
}
