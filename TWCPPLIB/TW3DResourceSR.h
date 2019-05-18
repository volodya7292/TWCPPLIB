#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

class TW3DResourceSR : public TW3DResource {
public:
	TW3DResourceSR(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TW3DTempGCL* TempGCL);
	~TW3DResourceSR();

	void Create2D(TWT::uint Width, TWT::uint Height, DXGI_FORMAT Format);
	void CreateArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth, DXGI_FORMAT Format);
	void Upload2D(TWT::byte* Data, TWT::int64 BytesPerRow, TWT::uint Depth = 0);
	void Upload2D(const TWT::WString& filename, TWT::uint Depth = 0);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

	static TW3DResourceSR* Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, const TWT::WString& filename, TW3DTempGCL* TempGCL);

private:
	TW3DTempGCL* TempGCL;
	TW3DDescriptorHeap* SRVDescriptorHeap;
	D3D12_RESOURCE_DESC ImageDesc = {};
	TWT::uint8* gpu_address = nullptr;
	int SRVIndex;
};