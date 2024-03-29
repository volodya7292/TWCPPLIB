#pragma once
#include "TWUtils.h"
#include "TWLogger.h"

#define TW3DCompiledShader(bytecode) CD3DX12_SHADER_BYTECODE((void*)bytecode, sizeof(bytecode))

namespace TWU {
	void TW3DSetLogger(TW::TWLogger* Logger);

	// Get the DXGI format equivilent of a WIC format
	DXGI_FORMAT WICFormatToDXGIFormat(WICPixelFormatGUID& wicFormatGUID);

	// Get a DXGI compatible WIC format from another WIC format
	WICPixelFormatGUID WICFormatToDXGICompatible(WICPixelFormatGUID& wicFormatGUID);

	// Get the number of bits per pixel for a DXGI format
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT dxgiFormat);

	TWT::String DXCommandListSupportFlagsStr(D3D12_COMMAND_LIST_SUPPORT_FLAGS Flags);

	D3D12_DESCRIPTOR_RANGE DXDescriptorRange(TWT::uint Register, D3D12_DESCRIPTOR_RANGE_TYPE Type);

	// Load and decode image from file
	int LoadImageDataFromFile(TWT::byte** imageData, D3D12_RESOURCE_DESC& resourceDescription, const TWT::WString& filename, int& bytesPerRow);

	// V - Vertex, F - Float, O - Offset
	void TW3DCalculateTriangleNormals(void* VertexBuffer, TWT::uint VCount, TWT::uint VFSize,
		TWT::uint VPosFO, TWT::uint VTexCoordFO, TWT::uint VNormalFO, TWT::uint VTangentFO);

	// Safely release DirectX resources
	template<typename T>
	void DXSafeRelease(T* t) {
		t->Release();
	}

	template<typename T>
	void DXSetName(T* t, TWT::String const& Name) {
#ifdef _DEBUG
		t->SetName(Name.Wide().ToWCharArray());
#endif // _DEBUG
	}

	// Check for successful result
	void SuccessAssert(HRESULT hr, const TWT::String& AdditionalErrorInfo);

	// Log info to logger
	void TW3DLogInfo(const TWT::String& Info);

	// Log error to logger
	void TW3DLogError(const TWT::String& Error);
}  // namespace TWU