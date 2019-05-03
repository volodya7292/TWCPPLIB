#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceCB : public TW3DResource {
	public:
		TW3DResourceCB(TW3DDevice* Device, TWT::UInt ElementCount, TWT::UInt ElementSize);
		~TW3DResourceCB();

		D3D12_GPU_VIRTUAL_ADDRESS GetAddress(TWT::UInt ElementIndex);

		void Update(void* Data, TWT::UInt ElementIndex);

	private:
		TWT::UInt8* GPUAddress = nullptr;
		TWT::UInt AlignedElementSize;
	};
}