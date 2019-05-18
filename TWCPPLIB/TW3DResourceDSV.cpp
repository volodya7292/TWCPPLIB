#include "pch.h"
#include "TW3DResourceDSV.h"

TW3DResourceDSV::TW3DResourceDSV(TW3DDevice* Device, TW3DDescriptorHeap* DSVDescriptorHeap) :
	TW3DResource(Device), DSVDescriptorHeap(DSVDescriptorHeap)
{
	DSVIndex = DSVDescriptorHeap->Allocate();
}

TW3DResourceDSV::~TW3DResourceDSV() {
	DSVDescriptorHeap->Free(DSVIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DResourceDSV::GetCPUHandle() {
	return DSVDescriptorHeap->GetCPUHandle(DSVIndex);
}

void TW3DResourceDSV::Create(TWT::uint Width, TWT::uint Height) {
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue ={};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&Resource,
		&depthOptimizedClearValue);
	Resource->SetName(L"TW3DResourceDSV");

	Device->CreateDepthStencilView(Resource, GetCPUHandle(), &desc);
}

void TW3DResourceDSV::Release() {
	TW3DResource::Release();
}