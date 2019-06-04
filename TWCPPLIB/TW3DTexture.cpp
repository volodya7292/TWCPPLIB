#include "pch.h"
#include "TW3DTexture.h"

TW3DTexture::TW3DTexture(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* DescriptorHeap, DXGI_FORMAT Format, bool UAV) :
	TW3DResource(Device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), TempGCL, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE), descriptor_heap(DescriptorHeap)
{
	main_index = DescriptorHeap->Allocate();
	if (UAV)
		uav_index = DescriptorHeap->Allocate();

	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = Format;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	uav_desc.Format = Format;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	dsv_desc.Format = Format;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
}

TW3DTexture::~TW3DTexture() {
	descriptor_heap->Free(main_index);
	descriptor_heap->Free(uav_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUSRVHandle() {
	return descriptor_heap->GetGPUHandle(main_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUUAVHandle() {
	return descriptor_heap->GetGPUHandle(uav_index);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DTexture::GetCPUHandle() {
	return descriptor_heap->GetCPUHandle(main_index);
}

void TW3DTexture::CreateDepthStencil(TWT::uint Width, TWT::uint Height) {
	clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	clear_value.DepthStencil.Depth = 1.0f;
	clear_value.DepthStencil.Stencil = 0;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	InitialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	TW3DResource::Create();
	resource->SetName(L"TW3DResourceDSV");

	device->CreateDepthStencilView(resource, descriptor_heap->GetCPUHandle(main_index), &dsv_desc);

	type = TW3D_TEXTURE_DEPTH_STENCIL;
}

void TW3DTexture::Create2D(TWT::uint Width, TWT::uint Height) {
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(srv_desc.Format, Width, Height, 1, 1, 1, 0, uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DResource::Create();
	resource->SetName(L"TW3DResourceSR 2D");

	device->CreateShaderResourceView(resource, &srv_desc, descriptor_heap->GetCPUHandle(main_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, descriptor_heap->GetCPUHandle(uav_index));

	type = TW3D_TEXTURE_2D;
}

void TW3DTexture::CreateArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth) {
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srv_desc.Texture2DArray.MipLevels = 1;
	srv_desc.Texture2DArray.ArraySize = Depth;
	srv_desc.Texture2DArray.FirstArraySlice = 0;
	srv_desc.Texture2DArray.MostDetailedMip = 0;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	uav_desc.Texture2DArray.ArraySize = Depth;
	uav_desc.Texture2DArray.FirstArraySlice = 0;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(srv_desc.Format, Width, Height, Depth, 1, 1, 0, uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DResource::Create();
	resource->SetName(L"TW3DResourceSR 2D Array");

	device->CreateShaderResourceView(resource, &srv_desc, descriptor_heap->GetCPUHandle(main_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, descriptor_heap->GetCPUHandle(uav_index));

	type = TW3D_TEXTURE_2D_ARRAY;
}

void TW3DTexture::Upload2D(TWT::byte* Data, TWT::int64 BytesPerRow, TWT::uint Depth) {
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &Data[0];
	textureData.RowPitch = BytesPerRow;
	textureData.SlicePitch = BytesPerRow * desc.Height;

	TW3DResource* upload_heap = staging;
	if (!staging)
		upload_heap = TW3DResource::CreateStaging(device, device->GetCopyableFootprints(&desc, 1));

	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(resource, upload_heap->Get(), &textureData, 1, 0, Depth);
	temp_gcl->ResourceBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	temp_gcl->Execute();

	if (!staging)
		delete upload_heap;
}

void TW3DTexture::Upload2D(TWT::WString const& filename, TWT::uint Depth) {
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	TWT::byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);
	Upload2D(imageData, imageBytesPerRow, Depth);
	delete imageData;
}

void TW3DTexture::Resize(TWT::uint Width, TWT::uint Height, TWT::uint Depth) {
	Release();

	switch (type) {
	case TW3D_TEXTURE_2D:
		Create2D(Width, Height);
		break;
	case TW3D_TEXTURE_2D_ARRAY:
		CreateArray2D(Width, Height, Depth);
		break;
	case TW3D_TEXTURE_DEPTH_STENCIL:
		CreateDepthStencil(Width, Height);
		break;
	}
}

TWT::vec2u TW3DTexture::GetSize() {
	return TWT::vec2u(desc.Width, desc.Height);
}

TW3DTexture* TW3DTexture::Create2D(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::WString const& filename) {
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	TWT::byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);

	TW3DTexture* texture = new TW3DTexture(Device, TempGCL, SRVDescriptorHeap, textureDesc.Format, false);
	texture->Create2D(static_cast<TWT::uint>(textureDesc.Width), static_cast<TWT::uint>(textureDesc.Height));
	texture->Upload2D(imageData, imageBytesPerRow);

	delete imageData;
	return texture;
}