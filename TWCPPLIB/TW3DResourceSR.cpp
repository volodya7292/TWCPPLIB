#include "pch.h"
#include "TW3DResourceSR.h"

TW3D::TW3DResourceSR::TW3DResourceSR(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap), TempGCL(TempGCL)
{	
	
}

TW3D::TW3DResourceSR::~TW3DResourceSR() {
	
}

void TW3D::TW3DResourceSR::Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
	SRVIndex = SRVDescriptorHeap->Allocate();

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	ImageDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&ImageDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		&Resource);

	Device->CreateShaderResourceView(Resource, &desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
}

void TW3D::TW3DResourceSR::Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow) {
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &Data[0]; // pointer to our image data
	textureData.RowPitch = BytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = BytesPerRow * ImageDesc.Height; // also the size of our triangle vertex data

	TWT::UInt64 textureUploadBufferSize = Device->GetCopyableFootprints(&ImageDesc, 1);

	ID3D12Resource* textureBufferUploadHeap;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&textureBufferUploadHeap);

	TempGCL->Reset();
	TempGCL->UpdateSubresources(Resource, textureBufferUploadHeap, &textureData);
	TempGCL->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	TempGCL->Execute();

	TWU::DXSafeRelease(textureBufferUploadHeap);
}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceSR::GetGPUHandle() {
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex);
}

TW3D::TW3DResourceSR* TW3D::TW3DResourceSR::Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TWT::WString filename, TW3DTempGCL* TempGCL) {
	D3D12_RESOURCE_DESC textureDesc;
	TWT::Int imageBytesPerRow;
	TWT::Byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);

	TW3DResourceSR* texture = new TW3DResourceSR(Device, DescriptorHeap, TempGCL);
	texture->Create2D(static_cast<UINT>(textureDesc.Width), textureDesc.Height, textureDesc.Format);
	texture->Upload2D(imageData, imageBytesPerRow);

	delete imageData;
	return texture;
}
