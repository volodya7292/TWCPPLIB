#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

class TW3DBuffer : public TW3DResource {
public:
	TW3DBuffer(TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint ElementSizeInBytes, bool UAV, TW3DDescriptorHeap* SRVDescriptorHeap);
	~TW3DBuffer() final;

	// Returns last updated elements count
	const TWT::uint GetElementCount() const;
	const TWT::uint GetMaxElementCount() const;

	void Create(TWT::uint MaxElementCount);
	void Update(const void* Data, TWT::uint ElementCount);
	void Update(const void* Data, TWT::uint ElementCount, D3D12_RESOURCE_STATES BeforeUpdate, D3D12_RESOURCE_STATES AfterUpdate);
	void UpdateElement(const void* Data, TWT::uint ElementIndex);

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC      srv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC     uav_desc = {};

	TWT::uint    element_size      = 0;
	TWT::uint    element_count     = 0;
	TWT::uint    max_element_count = 0;

	TW3DDescriptorHeap* srv_descriptor_heap;
	int srv_index = -1, uav_index = -1;
};

