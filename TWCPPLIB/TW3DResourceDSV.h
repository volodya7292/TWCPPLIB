#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceDSV {
	public:
		TW3DResourceDSV(TW3DDevice* Device);
		~TW3DResourceDSV();

		D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

		void Create(TWT::UInt Width, TWT::UInt Height);
		void Release();

	private:
		TW3DDevice* Device;
		TW3DDescriptorHeap* DescriptorHeap;
		TW3DResource* Buffer;
	};
}

