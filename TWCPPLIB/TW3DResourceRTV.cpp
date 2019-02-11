#include "pch.h"
#include "TW3DResourceRTV.h"

TW3D::TW3DResourceRTV::TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset) :
	TW3DResource(Device), RTVDescriptorHeap(RTVDescriptorHeap), RTVOffset(RTVOffset)
{
}

TW3D::TW3DResourceRTV::TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::Int SRVOffset) :
	TW3DResource(Device), RTVDescriptorHeap(RTVDescriptorHeap), RTVOffset(RTVOffset), SRVDescriptorHeap(SRVDescriptorHeap), SRVOffset(SRVOffset)
{
}

TW3D::TW3DResourceRTV::~TW3DResourceRTV() {
	
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceRTV::GetHandle() {
	return RTVDescriptorHeap->GetHandle(RTVOffset, Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

void TW3D::TW3DResourceRTV::Create(ID3D12Resource* Buffer) {
	Resource = Buffer;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetHandle();
	Device->CreateRenderTargetView(Resource, rtvHandle);
}

void TW3D::TW3DResourceRTV::Create(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	D3D12_RESOURCE_DESC ImageDesc = {};
	ImageDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ImageDesc.Width = Width;
	ImageDesc.Height = Height;
	ImageDesc.Format = Format;
	ImageDesc.MipLevels = 1;
	ImageDesc.DepthOrArraySize = 1;
	ImageDesc.Alignment = 0;
	ImageDesc.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	ImageDesc.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	ImageDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	ImageDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // no flags
	
	D3D12_CLEAR_VALUE clearValue = { Format, { 0.f, 0.f, 0.f, 1.f } };

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
		&ImageDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&Resource, &clearValue);

	Device->CreateShaderResourceView(Resource, &desc, SRVDescriptorHeap->GetHandle(SRVOffset, Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetHandle();
	Device->CreateRenderTargetView(Resource, rtvHandle);
}

void TW3D::TW3DResourceRTV::Release() {
	TW3DResource::Release();
}
