#pragma once
#include "TW3DResource.h"

enum TW3DTextureType {
	TW3D_TEXTURE_UNKNOWN,
	TW3D_TEXTURE_2D,
	TW3D_TEXTURE_2D_ARRAY,
	TW3D_TEXTURE_DEPTH_STENCIL
};

class TW3DTexture : public TW3DResource {
public:
	TW3DTexture(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* DescriptorHeap, DXGI_FORMAT Format, bool UAV = false);
	~TW3DTexture() final;

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();

	void CreateDepthStencil(TWT::uint Width, TWT::uint Height);
	void Create2D(TWT::uint Width, TWT::uint Height);
	void CreateArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth);
	void Upload2D(TWT::byte* Data, TWT::int64 BytesPerRow, TWT::uint Depth = 0);
	void Upload2D(TWT::WString const& filename, TWT::uint Depth = 0);
	void Resize(TWT::uint Width, TWT::uint Height, TWT::uint Depth = 1);

	TWT::vec2u GetSize();

	static TW3DTexture* Create2D(TW3DDevice* Device, TW3DTempGCL* TempGCL, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::WString const& filename);

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC      srv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC     uav_desc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC        dsv_desc = {};

	TW3DDescriptorHeap* descriptor_heap;
	TWT::uint8* gpu_address = nullptr;

	int main_index = -1, uav_index = -1;
	TW3DTextureType type = TW3D_TEXTURE_UNKNOWN;
};