#include "pch.h"
#include "TW3DResourceSV.h"

TW3D::TW3DResourceSV::TW3DResourceSV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap) :
	Device(Device), DescriptorHeap(DescriptorHeap)
{	
}

TW3D::TW3DResourceSV::~TW3DResourceSV() {
	delete Buffer;
}

void TW3D::TW3DResourceSV::Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
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

	Buffer = TW3D::TW3DResource::CreateTexture2D(Device, Format, Width, Height);
	Device->CreateShaderResourceView(Buffer->Get(), &desc, DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void TW3D::TW3DResourceSV::Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow) {
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &Data[0]; // pointer to our image data
	textureData.RowPitch = BytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = BytesPerRow * ImageDesc.Height; // also the size of our triangle vertex data

	TW3DFence* fence = new TW3DFence(Device);

	TWT::UInt64 textureUploadBufferSize = Device->GetCopyableFootprints(&ImageDesc, 1);
	TW3DResource* textureBufferUploadHeap = TW3DResource::Create(Device, textureUploadBufferSize, true);


	TW3DCommandQueue* commandQueue = new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

	ID3D12CommandAllocator* commandAllocator;
	Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, &commandAllocator);


	TW3DGraphicsCommandList* commandList = new TW3DGraphicsCommandList(Device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator);

	commandList->UpdateSubresources(Buffer, textureBufferUploadHeap, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	commandList->ResourceBarrier(Buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// Now we execute the command list to upload the initial assets (triangle data)
	commandList->Close();
	//ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandList(commandList);

	fence->Flush(commandQueue);

	delete commandList;
	commandAllocator->Release();
	delete commandQueue;
	delete fence;
	delete textureBufferUploadHeap;
}
