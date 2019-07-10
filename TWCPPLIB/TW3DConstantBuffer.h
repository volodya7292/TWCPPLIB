#pragma once
#include "TW3DResource.h"

class TW3DConstantBuffer : public TW3DResource {
public:
	TW3DConstantBuffer(TWT::String Name, TW3DDevice* Device, TWT::uint ElementCount, TWT::uint ElementSize);
	~TW3DConstantBuffer() final = default;

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(TWT::uint ElementIndex);

	void Update(void* Data, TWT::uint ElementIndex = 0);

private:
	TWT::uint8* GPUAddress = nullptr;
	TWT::uint AlignedElementSize;
};