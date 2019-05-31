#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

class TW3DBuffer : public TW3DResource {
public:
	TW3DBuffer(TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint ElementSizeInBytes, bool UAV, TW3DDescriptorHeap* SRVDescriptorHeap);
	~TW3DBuffer() final;

	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();
	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUUAVHandle();
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAVHandle();
	TWT::uint GetElementCount();

	void Create(TWT::uint ElementCount);
	void Update(const void* Data, TWT::uint ElementCount);
	void UpdateElement(const void* Data, TWT::uint ElementIndex);

private:
	TWT::uint8* staging_addr;

	D3D12_SHADER_RESOURCE_VIEW_DESC      srv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC     uav_desc = {};

	TWT::uint    element_size = 0;
	TWT::uint    element_count = 0;

	TW3DDescriptorHeap* srv_descriptor_heap;
	int srv_index = -1, uav_index = -1;
};

