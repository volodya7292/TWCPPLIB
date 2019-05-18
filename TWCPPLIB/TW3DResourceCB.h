#pragma once
#include "TW3DResource.h"

class TW3DResourceCB : public TW3DResource {
public:
	TW3DResourceCB(TW3DDevice* Device, TWT::uint ElementCount, TWT::uint ElementSize);
	~TW3DResourceCB();

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress(TWT::uint ElementIndex);

	void Update(void* Data, TWT::uint ElementIndex);

private:
	TWT::uint8* GPUAddress = nullptr;
	TWT::uint AlignedElementSize;
};