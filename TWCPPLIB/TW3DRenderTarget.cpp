#include "pch.h"
#include "TW3DRenderTarget.h"

TW3DRenderTarget::TW3DRenderTarget(TW3DDevice* Device, TW3DDescriptorHeap* rtv_descriptor_heap) :
	TW3DResource(Device), rtv_descriptor_heap(rtv_descriptor_heap)
{
	RTVIndex = rtv_descriptor_heap->Allocate();
}

TW3DRenderTarget::TW3DRenderTarget(TW3DDevice* Device, TW3DDescriptorHeap* rtv_descriptor_heap, TW3DDescriptorHeap* srv_descriptor_heap, DXGI_FORMAT Format, TWT::vec4 ClearValue) :
	TW3DResource(Device, nullptr,
	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	D3D12_RESOURCE_STATE_RENDER_TARGET,
	false,
	D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES),
	rtv_descriptor_heap(rtv_descriptor_heap), srv_descriptor_heap(srv_descriptor_heap)
{
	RTVIndex = rtv_descriptor_heap->Allocate();
	SRVIndex = srv_descriptor_heap->Allocate();
	desc = CD3DX12_RESOURCE_DESC::Tex2D(Format, 0, 0, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	clear_value = { Format, { ClearValue.r, ClearValue.g, ClearValue.b, ClearValue.a } };
}

TW3DRenderTarget::~TW3DRenderTarget() {
	rtv_descriptor_heap->Free(RTVIndex);
	srv_descriptor_heap->Free(SRVIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DRenderTarget::GetCPURTVHandle() {
	return rtv_descriptor_heap->GetCPUHandle(RTVIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DRenderTarget::GetGPUSRVHandle() {
	return srv_descriptor_heap->GetGPUHandle(SRVIndex);
}

void TW3DRenderTarget::Create(ID3D12Resource* Buffer) {
	resource = Buffer;
	device->CreateRenderTargetView(resource, GetCPURTVHandle());
}

void TW3DRenderTarget::Create(TWT::uint Width, TWT::uint Height) {
	desc.Width = Width;
	desc.Height = Height;
	
	TW3DResource::Create();
	resource->SetName(L"TW3DResourceRTV");

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(resource, &srvdesc, srv_descriptor_heap->GetCPUHandle(SRVIndex));
	device->CreateRenderTargetView(resource, GetCPURTVHandle());
}

void TW3DRenderTarget::Resize(TWT::uint Width, TWT::uint Height) {
	Release();
	Create(Width, Height);
}
