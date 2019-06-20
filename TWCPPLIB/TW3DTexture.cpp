#include "pch.h"
#include "TW3DTexture.h"

TW3DTexture::TW3DTexture(TW3DDevice* Device, TW3DTempGCL* TempGCL, DXGI_FORMAT Format,
		TW3DDescriptorHeap* SRVDescriptorHeap, TW3DDescriptorHeap* UAVDescriptorHeap, TW3DDescriptorHeap* DSVDescriptorHeap) :
	TW3DResource(Device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), TempGCL, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
	srv_descriptor_heap(SRVDescriptorHeap), uav_descriptor_heap(UAVDescriptorHeap), dsv_descriptor_heap(DSVDescriptorHeap) {

	if (SRVDescriptorHeap)
		srv_index = srv_descriptor_heap->Allocate();
	if (UAVDescriptorHeap) {
		uav_cpu_index = uav_descriptor_heap->Allocate();
		uav_index = srv_descriptor_heap->Allocate();
	}
	if (DSVDescriptorHeap)
		dsv_index = dsv_descriptor_heap->Allocate();

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
	if (srv_descriptor_heap)
		srv_descriptor_heap->Free(srv_index);
	if (uav_descriptor_heap) {
		uav_descriptor_heap->Free(uav_index);
		srv_descriptor_heap->Free(uav_cpu_index);
	}
	if (dsv_descriptor_heap)
		dsv_descriptor_heap->Free(dsv_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUSRVHandle() {
	return srv_descriptor_heap->GetGPUHandle(srv_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUCPUUAVHandle() {
	return uav_descriptor_heap->GetGPUHandle(uav_cpu_index);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DTexture::GetGPUUAVHandle() {
	return srv_descriptor_heap->GetGPUHandle(uav_index);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DTexture::GetCPUSRVHandle() {
	return srv_descriptor_heap->GetCPUHandle(srv_index);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DTexture::GetCPUUAVHandle() {
	return uav_descriptor_heap->GetCPUHandle(uav_cpu_index);
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3DTexture::GetCPUDSVHandle() {
	return dsv_descriptor_heap->GetCPUHandle(dsv_index);
}

void TW3DTexture::CreateDepthStencil(TWT::uint2 Size) {
	clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	clear_value.DepthStencil.Depth = 1.0f;
	clear_value.DepthStencil.Stencil = 0;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Size.x, Size.y, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	InitialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	TW3DResource::Create();
	resource->SetName(L"TW3DResourceDSV");

	device->CreateDepthStencilView(resource, dsv_descriptor_heap->GetCPUHandle(dsv_index), &dsv_desc);

	type = TW3D_TEXTURE_DEPTH_STENCIL;
}

void TW3DTexture::Create2D(TWT::uint2 Size) {
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(srv_desc.Format, Size.x, Size.y, 1, 1, 1, 0, uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DResource::Create();
	resource->SetName(L"TW3DResourceSR 2D");

	device->CreateShaderResourceView(resource, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1) {
		device->CreateUnorderedAccessView(resource, &uav_desc, srv_descriptor_heap->GetCPUHandle(uav_index));
		device->CreateUnorderedAccessView(resource, &uav_desc, uav_descriptor_heap->GetCPUHandle(uav_cpu_index));
	}

	type = TW3D_TEXTURE_2D;
}

void TW3DTexture::CreateArray2D(TWT::uint2 Size, TWT::uint Depth) {
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srv_desc.Texture2DArray.MipLevels = 1;
	srv_desc.Texture2DArray.ArraySize = Depth;
	srv_desc.Texture2DArray.FirstArraySlice = 0;
	srv_desc.Texture2DArray.MostDetailedMip = 0;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	uav_desc.Texture2DArray.ArraySize = Depth;
	uav_desc.Texture2DArray.FirstArraySlice = 0;

	desc = CD3DX12_RESOURCE_DESC::Tex2D(srv_desc.Format, Size.x, Size.y, Depth, 1, 1, 0, uav_index == -1 ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DResource::Create();
	resource->SetName(L"TW3DResourceSR 2D Array");

	device->CreateShaderResourceView(resource, &srv_desc, srv_descriptor_heap->GetCPUHandle(srv_index));
	if (uav_index != -1)
		device->CreateUnorderedAccessView(resource, &uav_desc, uav_descriptor_heap->GetCPUHandle(uav_index));

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

	TWT::uint subres = D3D12CalcSubresource(0, Depth, 0, desc.MipLevels, desc.DepthOrArraySize);
	
	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	temp_gcl->UpdateSubresources(resource, upload_heap->Get(), &textureData, 1, 0, subres);
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

void TW3DTexture::Resize(TWT::uint2 Size, TWT::uint Depth) {
	Release();

	switch (type) {
	case TW3D_TEXTURE_2D:
		Create2D(Size);
		break;
	case TW3D_TEXTURE_2D_ARRAY:
		CreateArray2D(Size, Depth);
		break;
	case TW3D_TEXTURE_DEPTH_STENCIL:
		CreateDepthStencil(Size);
		break;
	}
}

TWT::uint2 TW3DTexture::GetSize() {
	return TWT::uint2(desc.Width, desc.Height);
}

TW3DTexture* TW3DTexture::Create2D(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::WString const& filename) {
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	TWT::byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);

	TW3DTexture* texture = new TW3DTexture(Device, TempGCL, textureDesc.Format, SRVDescriptorHeap, nullptr, nullptr);
	texture->Create2D(TWT::uint2(static_cast<TWT::uint>(textureDesc.Width), static_cast<TWT::uint>(textureDesc.Height)));
	texture->Upload2D(imageData, imageBytesPerRow);

	delete imageData;
	return texture;
}