#include "pch.h"
#include "TW3DResource.h"

TW3D::TW3DResource::TW3DResource(TW3DDevice* Device, const CD3DX12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags,
	const D3D12_RESOURCE_DESC* ResourceDescription, D3D12_RESOURCE_STATES ResourceStates, const D3D12_CLEAR_VALUE* OptimizedClearValue) {

	Device->CreateCommittedResource(HeapProperties, HeapFlags, ResourceDescription, ResourceStates, &resource, OptimizedClearValue);
}

TW3D::TW3DResource::TW3DResource(ID3D12Resource* resource) : 
	resource(resource)
{
}

TW3D::TW3DResource::~TW3DResource() {
	TWU::DXSafeRelease(resource);
}

ID3D12Resource* TW3D::TW3DResource::Get() {
	return resource;
}

D3D12_GPU_VIRTUAL_ADDRESS TW3D::TW3DResource::GetGPUVirtualAddress() {
	return resource->GetGPUVirtualAddress();
}

void TW3D::TW3DResource::Map(TWT::UInt SubResourceIndex, D3D12_RANGE* ReadRange, void** Data) {
	TWU::ThrowIfFailed(resource->Map(SubResourceIndex, ReadRange, Data));
}

TW3D::TW3DResource* TW3D::TW3DResource::Create(TW3DDevice* Device, TWT::UInt64 Size, TWT::Bool Staging) {
	return new TW3DResource(Device,
		&CD3DX12_HEAP_PROPERTIES(Staging ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		Staging ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST
	);
}

TW3D::TW3DResource* TW3D::TW3DResource::CreateCBStaging(TW3DDevice* Device) {
	return Create(Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, true);
}

TW3D::TW3DResource* TW3D::TW3DResource::CreateDS(TW3DDevice* Device, TWT::UInt Width, TWT::UInt Height) {
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	return new TW3DResource(Device,
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue
	);
}

TW3D::TW3DResource* TW3D::TW3DResource::CreateTexture2D(TW3DDevice* Device, DXGI_FORMAT Format, TWT::UInt Width, TWT::UInt Height) {
	D3D12_RESOURCE_DESC resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	resourceDescription.Width = Width; // width of the texture
	resourceDescription.Height = Height; // height of the texture
	resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	resourceDescription.Format = Format; // This is the dxgi format of the image (format of the pixels)
	resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

	return new TW3DResource(Device,
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&resourceDescription,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
}

TW3D::TW3DResource* TW3D::TW3DResource::CreateDSView(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap) {
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	ID3D12Resource* res = nullptr;
	Device->CreateDepthStencilView(res, DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), &depthStencilDesc);

	return new TW3DResource(res);
}

TW3D::TW3DResource* TW3D::TW3DResource::CreateShaderView(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, DXGI_FORMAT Format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ID3D12Resource* res = nullptr;
	Device->CreateShaderResourceView(res, &srvDesc, DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	return new TW3DResource(res);
}
