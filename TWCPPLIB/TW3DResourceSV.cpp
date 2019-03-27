#include "pch.h"
#include "TW3DResourceSV.h"

TW3D::TW3DResourceSV::TW3DResourceSV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), DescriptorHeap(DescriptorHeap), TempGCL(TempGCL)
{	
	
}

TW3D::TW3DResourceSV::~TW3DResourceSV() {
	
}

void TW3D::TW3DResourceSV::Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Int HeapIndex) {
	this->HeapIndex = HeapIndex;

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format = Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

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
	ImageDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags
	
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&ImageDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		&Resource);

	Device->CreateShaderResourceView(Resource, &desc, DescriptorHeap->GetCPUHandle(HeapIndex));
}

void TW3D::TW3DResourceSV::Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow) {
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

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceSV::GetGPUHandle() {
	return DescriptorHeap->GetGPUHandle(HeapIndex);
}

TW3D::TW3DResourceSV* TW3D::TW3DResourceSV::Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TWT::WString filename, TW3DTempGCL* TempGCL, TWT::Int HeapIndex) {
	D3D12_RESOURCE_DESC textureDesc;
	TWT::Int imageBytesPerRow;
	TWT::Byte* imageData;
	int imageSize = TWU::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);

	TW3DResourceSV* texture = new TW3DResourceSV(Device, DescriptorHeap, TempGCL);
	texture->Create2D(static_cast<UINT>(textureDesc.Width), textureDesc.Height, textureDesc.Format, HeapIndex);
	texture->Upload2D(imageData, imageBytesPerRow);

	delete imageData;
	return texture;
}
