#pragma once
#include "TW3DResource.h"
#include "TW3DTypes.h"

class TW3DResourceRTV : public TW3DResource {
public:
	TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap);
	TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, TWT::vec4 ClearValue);
	~TW3DResourceRTV();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();
	TWT::vec4 GetClearColor();

	void Create(ID3D12Resource* Buffer);
	void Create(TWT::uint Width, TWT::uint Height);
	void Release();

private:
	TW3DDescriptorHeap*	RTVDescriptorHeap;
	TW3DDescriptorHeap* SRVDescriptorHeap = nullptr;
	D3D12_RESOURCE_DESC ImageDesc = {};
	TWT::vec4       ClearValue;
	int RTVIndex = -1, SRVIndex = -1;
};