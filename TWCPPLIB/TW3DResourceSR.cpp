#include "pch.h"
#include "TW3DResourceSR.h"

TW3DResourceSR::TW3DResourceSR(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap), TempGCL(TempGCL)
{	
	SRVIndex = SRVDescriptorHeap->Allocate();
}

TW3DResourceSR::~TW3DResourceSR() {
	SRVDescriptorHeap->Free(SRVIndex);
}

void TW3DResourceSR::Create2D(TWT::uint Width, TWT::uint Height, DXGI_FORMAT Format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	ImageDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, Width, Height, 1, 1, 1, 0);
	
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&ImageDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&Resource);
	Resource->SetName(L"TW3DResourceSR 2D");

	Device->CreateShaderResourceView(Resource, &desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
}

void TW3DResourceSR::CreateArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth, DXGI_FORMAT Format) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.ArraySize = Depth;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MostDetailedMip = 0;

	ImageDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, Width, Height, Depth, 1, 1, 0);

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&ImageDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&Resource);
	Resource->SetName(L"TW3DResourceSR 2D Array");

	Device->CreateShaderResourceView(Resource, &desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
}

void TW3DResourceSR::Upload2D(TWT::byte* Data, TWT::int64 BytesPerRow, TWT::uint Depth) {
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &Data[0];
	textureData.RowPitch = BytesPerRow;
	textureData.SlicePitch = BytesPerRow * ImageDesc.Height;

	TWT::uint64 textureUploadBufferSize = Device->GetCopyableFootprints(&ImageDesc, 1);

	ID3D12Resource* textureBufferUploadHeap;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&textureBufferUploadHeap);
	Resource->SetName(L"TW3DResource Upload 2D Heap");

	TempGCL->Reset();
	TempGCL->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	TempGCL->UpdateSubresources(Resource, textureBufferUploadHeap, &textureData, 1, 0, Depth);
	TempGCL->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	TempGCL->Execute();

	TWU::DXSafeRelease(textureBufferUploadHeap);
}

void TW3DResourceSR::Upload2D(const TWT::WString& filename, TWT::uint Depth) {
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	TWT::byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);
	Upload2D(imageData, imageBytesPerRow, Depth);
	delete imageData;
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3DResourceSR::GetGPUHandle() {
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex);
}

TW3DResourceSR* TW3DResourceSR::Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, const TWT::WString& filename, TW3DTempGCL* TempGCL) {
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	TWT::byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);

	TW3DResourceSR* texture = new TW3DResourceSR(Device, DescriptorHeap, TempGCL);
	texture->Create2D(static_cast<UINT>(textureDesc.Width), textureDesc.Height, textureDesc.Format);
	texture->Upload2D(imageData, imageBytesPerRow);

	delete imageData;
	return texture;
}
