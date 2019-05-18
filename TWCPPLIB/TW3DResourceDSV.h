#pragma once
#include "TW3DResource.h"

class TW3DResourceDSV : public TW3DResource {
public:
	TW3DResourceDSV(TW3DDevice* Device, TW3DDescriptorHeap* DSVDescriptorHeap);
	~TW3DResourceDSV();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();

	void Create(TWT::uint Width, TWT::uint Height);
	void Release();

private:
	TW3DDescriptorHeap* DSVDescriptorHeap;
	int DSVIndex = 0;
};