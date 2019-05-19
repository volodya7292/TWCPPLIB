#pragma once
#include "TW3DResource.h"
#include "TW3DTypes.h"

class TW3DRenderTarget : public TW3DResource {
public:
	TW3DRenderTarget(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap);
	TW3DRenderTarget(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, TWT::vec4 ClearValue);
	~TW3DRenderTarget();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();

	void Create(ID3D12Resource* Buffer);
	void Create(TWT::uint Width, TWT::uint Height);
	void Release();

private:
	TW3DDescriptorHeap*	rtv_descriptor_heap;
	TW3DDescriptorHeap* srv_descriptor_heap = nullptr;
	int RTVIndex = -1, SRVIndex = -1;
};