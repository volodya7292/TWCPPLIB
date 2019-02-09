#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceCB : public TW3DResource {
	public:
		TW3DResourceCB(TW3DDevice* Device, TWT::UInt ElementSize, TWT::UInt ElementCount);
		~TW3DResourceCB();

		TWT::UInt GetAddress(TWT::UInt ElementIndex);

		void Update(void* Data, TWT::UInt ElementIndex);

	private:
		TWT::UInt8* GPUAddress;
		TWT::UInt AlignedElementSize;
	};
}