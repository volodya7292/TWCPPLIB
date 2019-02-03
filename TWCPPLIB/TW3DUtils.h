#pragma once
#include "TWUtils.h"

namespace TWU {
	// Get the DXGI format equivilent of a WIC format
	DXGI_FORMAT WICFormatToDXGIFormat(WICPixelFormatGUID& wicFormatGUID);

	// Get a DXGI compatible WIC format from another WIC format
	WICPixelFormatGUID WICFormatToDXGICompatible(WICPixelFormatGUID& wicFormatGUID);

	// Get the number of bits per pixel for a DXGI format
	TWT::Int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);

	// Load and decode image from file
	TWT::Int LoadImageDataFromFile(TWT::Byte** imageData, D3D12_RESOURCE_DESC& resourceDescription, TWT::WString filename, TWT::Int& bytesPerRow);

	// Safely release DirectX resources
	template<typename T>
	void DXSafeRelease(T* t){
		t->Release();
	}

	void ThrowIfFailed(HRESULT hr);
}