#include "pch.h"
#include "TW3DResourceRTV.h"

TW3D::TW3DResourceRTV::TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVIndex) :
	TW3DResource(Device), RTVDescriptorHeap(RTVDescriptorHeap), RTVIndex(RTVIndex)
{
}

TW3D::TW3DResourceRTV::TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVIndex,
	TW3DDescriptorHeap* SRVDescriptorHeap, TWT::Int SRVIndex, DXGI_FORMAT Format, TWT::Vector4f ClearValue) :
	TW3DResource(Device), RTVDescriptorHeap(RTVDescriptorHeap), RTVIndex(RTVIndex), SRVDescriptorHeap(SRVDescriptorHeap), SRVIndex(SRVIndex), ClearValue(ClearValue)
{
	ImageDesc.Format = Format;
}

TW3D::TW3DResourceRTV::~TW3DResourceRTV() {
	
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceRTV::GetRTVCPUHandle() {
	return RTVDescriptorHeap->GetCPUHandle(RTVIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceRTV::GetSRVGPUHandle() {
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex);
}

TWT::Vector4f TW3D::TW3DResourceRTV::GetClearColor() {
	return ClearValue;
}

void TW3D::TW3DResourceRTV::Create(ID3D12Resource* Buffer) {
	Resource = Buffer;
	Device->CreateRenderTargetView(Resource, GetRTVCPUHandle());
}

void TW3D::TW3DResourceRTV::Create(TWT::UInt Width, TWT::UInt Height) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = ImageDesc.Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	ImageDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ImageDesc.Width = Width;
	ImageDesc.Height = Height;
	ImageDesc.MipLevels = 1;
	ImageDesc.DepthOrArraySize = 1;
	ImageDesc.Alignment = 0;
	ImageDesc.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	ImageDesc.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	ImageDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	ImageDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // no flags
	
	D3D12_CLEAR_VALUE optClearValue = { ImageDesc.Format, { ClearValue.x, ClearValue.y, ClearValue.z, ClearValue.w } };
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
		&ImageDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&Resource, &optClearValue);

	Device->CreateShaderResourceView(Resource, &desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetRTVCPUHandle();
	Device->CreateRenderTargetView(Resource, rtvHandle);
}

void TW3D::TW3DResourceRTV::Release() {
	TW3DResource::Release();
}
