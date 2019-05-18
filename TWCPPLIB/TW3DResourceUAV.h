#pragma once
#include "TW3DResource.h"
//#include "TW3DTempGCL.h"

class TW3DTempGCL;
class TW3DResourceUAV : public TW3DResource {
public:
	TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::UInt ElementSizeInBytes, TW3DTempGCL* TempGCL);
	TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format);
	~TW3DResourceUAV();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUUAVHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAVHandle();
	TWT::UInt GetElementCount();

	void UpdateData(const void* Data, TWT::UInt ElementCount);
	void Read(void* Out, TWT::UInt ByteOffset, TWT::UInt ByteCount);

	void CreateBuffer(TWT::UInt ElementCount);
	void CreateTexture2D(TWT::UInt Width, TWT::UInt Height);

	TW3DDescriptorHeap* SRVDescriptorHeap;
	TWT::Int SRVIndex = -1, UAVIndex = -1;

private:
	TW3DTempGCL* temp_gcl  = nullptr;

	DXGI_FORMAT Format = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC      srv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC     uav_desc = {};
	TWT::UInt                        element_size = 0;
	TWT::UInt                       element_count = 0;
};

D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource = nullptr);