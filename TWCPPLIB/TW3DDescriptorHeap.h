#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DDescriptorHeap {
	public:
		TW3DDescriptorHeap(TW3DDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, TWT::UInt count);
		~TW3DDescriptorHeap();

		ID3D12DescriptorHeap* Get();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();

		static TW3DDescriptorHeap* CreateForRTV(TW3DDevice* device, TWT::UInt count);
		static TW3DDescriptorHeap* CreateForDSV(TW3DDevice* device);
		static TW3DDescriptorHeap* CreateForSR(TW3DDevice* device, TWT::UInt count);

	private:
		ID3D12DescriptorHeap* descriptorHeap;
	};
}