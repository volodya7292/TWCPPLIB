#pragma once
#include "TW3DResource.h"
class TW3DTexture : public TW3DResource {
public:
	TW3DTexture(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, bool UAV);
	~TW3DTexture();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAVHandle();

	void Create2D(TWT::uint Width, TWT::uint Height);
	void CreateArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth);
	void Upload2D(TWT::byte* Data, TWT::int64 BytesPerRow, TWT::uint Depth = 0);
	void Upload2D(TWT::WString const& filename, TWT::uint Depth = 0);

	static TW3DTexture* Create2D(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::WString const& filename);

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC      srv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC     uav_desc = {};

	TW3DDescriptorHeap* srv_descriptor_heap;
	TWT::uint8* gpu_address = nullptr;

	int srv_index = -1, uav_index = -1;
};