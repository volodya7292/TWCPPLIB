#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceRTV {
	public:
		TW3DResourceRTV(TW3DDevice* Device);
		~TW3DResourceRTV();

		void Create();
		void Release();

	private:
		TW3DDevice*   Device;
		TW3DResource* Buffer;
	};
}