#include "pch.h"
#include "TW3DRenderTarget.h"

TW3DRenderTarget::TW3DRenderTarget(TWT::String Name, TW3DDevice* Device, TW3DDescriptorHeap* rtv_descriptor_heap, TW3DDescriptorHeap* srv_descriptor_heap) :
	TW3DResource(Name, Device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)), rtv_descriptor_heap(rtv_descriptor_heap), srv_descriptor_heap(srv_descriptor_heap)
{
	RTVIndex = rtv_descriptor_heap->Allocate();
	SRVIndex = srv_descriptor_heap->Allocate();
}

TW3DRenderTarget::TW3DRenderTarget(TWT::String Name, TW3DDevice* Device, TW3DDescriptorHeap* rtv_descriptor_heap, TW3DDescriptorHeap* srv_descriptor_heap, DXGI_FORMAT Format, TWT::float4 ClearValue) :
	TW3DResource(Name, Device,
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), nullptr,
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
	Native = Buffer;
	desc = Buffer->GetDesc();

	desc.Flags |= ResourceFlags;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(Native, &srvdesc, srv_descriptor_heap->GetCPUHandle(SRVIndex));
	device->CreateRenderTargetView(Native, GetCPURTVHandle());
}

void TW3DRenderTarget::Create(TWT::uint2 Size) {
	desc.Width = Size.x;
	desc.Height = Size.y;
	desc.Flags |= ResourceFlags;

	TW3DResource::Create();
	Native->SetName(L"TW3DResourceRTV");

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(Native, &srvdesc, srv_descriptor_heap->GetCPUHandle(SRVIndex));
	device->CreateRenderTargetView(Native, GetCPURTVHandle());

	type = TW3D_RENDER_TARGET_2D;
}

void TW3DRenderTarget::CreateCube(TWT::uint Size) {
	desc.Width = Size;
	desc.Height = Size;
	desc.DepthOrArraySize = 6;
	desc.Flags |= ResourceFlags;

	TW3DResource::Create();
	Native->SetName(L"TW3DResourceRTV");

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(Native, &srvdesc, srv_descriptor_heap->GetCPUHandle(SRVIndex));
	device->CreateRenderTargetView(Native, GetCPURTVHandle());

	type = TW3D_RENDER_TARGET_CUBE;
}

void TW3DRenderTarget::Resize(TWT::uint2 Size) {
	if (GetSize() != Size) {
		Release();

		switch (type) {
		case TW3D_RENDER_TARGET_2D:
			Create(Size);
		break; case TW3D_RENDER_TARGET_CUBE:
			CreateCube(Size.x);
		}
	}
}

TWT::uint2 TW3DRenderTarget::GetSize() {
	return TWT::uint2(desc.Width, desc.Height);
}
