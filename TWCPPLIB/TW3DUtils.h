#pragma once
#include "TWUtils.h"
#include "TWLogger.h"

#define TW3DCompiledShader(bytecode) CD3DX12_SHADER_BYTECODE((void*)bytecode, sizeof(bytecode))

namespace TWU {
	void TW3DSetLogger(TW::TWLogger* Logger);

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
	TWT::Int GetDXGIFormatBitsPerPixel(DXGI_FORMAT dxgiFormat);

	D3D12_DESCRIPTOR_RANGE DXDescriptorRange(TWT::UInt Register, D3D12_DESCRIPTOR_RANGE_TYPE Type);

	// Implementation of UpdateSubresources from d3dx12.h. Used in TW3DGraphicsCommandList.
	void UpdateSubresourcesImp(ID3D12GraphicsCommandList* commandList, ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
		D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource);

	// Load and decode image from file
	TWT::Int LoadImageDataFromFile(TWT::Byte** imageData, D3D12_RESOURCE_DESC& resourceDescription, const TWT::WString& filename, TWT::Int& bytesPerRow);

	void TW3DCalculateTriangleNormals(void* VertexBuffer, TWT::UInt VertexCount, TWT::UInt VertexFloatSize, TWT::UInt VertexPositionFloatOffset, TWT::UInt VertexNormalFloatOffset);

	// Safely release DirectX resources
	template<typename T>
	void DXSafeRelease(T* t){
		t->Release();
	}

	// Check for successful result
	void SuccessAssert(HRESULT hr);

	// Log info to logger
	void TW3DLogInfo(const TWT::String& Info);

	// Log error to logger
	void TW3DLogError(const TWT::String& Error);
}