#include "pch.h"
#include "TW3DResourceDSV.h"

TW3D::TW3DResourceDSV::TW3DResourceDSV(TW3DDevice* Device) :
	Device(Device)
{
	DescriptorHeap = TW3D::TW3DDescriptorHeap::CreateForDSV(Device);
}

TW3D::TW3DResourceDSV::~TW3DResourceDSV() {
	Release();
	delete DescriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceDSV::GetHandle() {
	return DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

void TW3D::TW3DResourceDSV::Create(TWT::UInt Width, TWT::UInt Height) {
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_DSV_FLAG_NONE;

	Buffer = TW3D::TW3DResource::CreateDS(Device, Width, Height);
	Device->CreateDepthStencilView(Buffer->Get(), GetHandle(), &desc);
}

void TW3D::TW3DResourceDSV::Release() {
	delete Buffer;
}