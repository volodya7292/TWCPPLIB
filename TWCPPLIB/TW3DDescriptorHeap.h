#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DDescriptorHeap {
	public:
		TW3DDescriptorHeap(TW3DDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, TWT::UInt count);
		~TW3DDescriptorHeap();

		static TW3DDescriptorHeap* createForRTV(TW3DDevice* device, TWT::UInt count);
		static TW3DDescriptorHeap* createForDSV(TW3DDevice* device);
		static TW3DDescriptorHeap* createForSR(TW3DDevice* device, TWT::UInt count);

	private:
		ID3D12DescriptorHeap* descriptorHeap;
	};
}