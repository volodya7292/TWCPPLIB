#include "pch.h"
#include "TW3DTexture.h"

TW3DTexture::TW3DTexture(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, bool UAV) :
	TW3DResource(Device, TempGCL, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE), srv_descriptor_heap(SRVDescriptorHeap)
{
	srv_index = SRVDescriptorHeap->Allocate();
	if (UAV)
		uav_index = SRVDescriptorHeap->Allocate();

	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = Format;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	uav_desc.Format = Format;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
}

TW3DTexture::~TW3DTexture() {
	srv_descriptor_heap->Free(srv_index);
	srv_descriptor_heap->Free(uav_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUSRVHandle() {
	return srv_descriptor_heap->GetGPUHandle(srv_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUUAVHandle() {
	return srv_descriptor_heap->GetGPUHandle(uav_index);
}

void TW3DTexture::Create2D(TWT::uint Width, TWT::uint Height) {
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(srv_desc.Format, Width, Height, 1, 1, 1, 0, uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DResource::Create();
	resource->SetName(L"TW3DResourceSR 2D");

	device->CreateShaderResourceView(resource, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, srv_descriptor_heap->GetCPUHandle(uav_index));
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

	device->CreateShaderResourceView(resource, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, srv_descriptor_heap->GetCPUHandle(uav_index));
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
