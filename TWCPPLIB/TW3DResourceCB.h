#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceCB : public TW3DResource {
	public:
		TW3DResourceCB(TW3DDevice* Device);
		~TW3DResourceCB();

		void Update(void* Data, TWT::UInt Size, TWT::UInt Align = 0);

	private:
		TWT::UInt8* GPUAddress;
	};
}