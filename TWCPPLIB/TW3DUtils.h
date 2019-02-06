#pragma once
#include "TWUtils.h"

namespace TWU {
	// Create a DXGI factory
	IDXGIFactory7* DXGICreateFactory(TWT::UInt flags);

	// Get available DirectX hardware adapter
	IDXGIAdapter4* DXGIGetHardwareAdapter(IDXGIFactory7* factory);

	// Create a DX12 device
	ID3D12Device5* DXCreateDevice(IDXGIAdapter4* adapter);

	// Get the DXGI format equivilent of a WIC format
	DXGI_FORMAT WICFormatToDXGIFormat(WICPixelFormatGUID& wicFormatGUID);

	// Get a DXGI compatible WIC format from another WIC format
	WICPixelFormatGUID WICFormatToDXGICompatible(WICPixelFormatGUID& wicFormatGUID);

	// Get the number of bits per pixel for a DXGI format
	TWT::Int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);

	// Implementation of UpdateSubresources from d3dx12.h. Used in TW3DGraphicsCommandList.
	void UpdateSubresourcesImp(ID3D12GraphicsCommandList* commandList, ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
		D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource);

	// Load and decode image from file
	TWT::Int LoadImageDataFromFile(TWT::Byte** imageData, D3D12_RESOURCE_DESC& resourceDescription, TWT::WString filename, TWT::Int& bytesPerRow);

	// Read entire file into byte array
	TWT::Byte* ReadFileBytes(std::string file, TWT::Int& size);

	// Safely release DirectX resources
	template<typename T>
	void DXSafeRelease(T* t){
		t->Release();
	}

	void ThrowIfFailed(HRESULT hr);
}