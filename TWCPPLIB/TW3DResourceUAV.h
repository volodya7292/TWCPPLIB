#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceUAV : public TW3DResource {
	public:
		TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format);
		~TW3DResourceUAV();

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

		void CreateTexture2D(TWT::UInt Width, TWT::UInt Height);

	private:
		TW3DDescriptorHeap* SRVDescriptorHeap;
		TWT::Int SRVIndex = 0;
		D3D12_RESOURCE_DESC desc;
	};
}