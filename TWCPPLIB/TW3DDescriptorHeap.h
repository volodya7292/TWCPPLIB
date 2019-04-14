#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DDescriptorHeap {
	public:
		TW3DDescriptorHeap(TW3DDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, TWT::UInt count);
		~TW3DDescriptorHeap();

		ID3D12DescriptorHeap* Get();
		TWT::Int Allocate();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(TWT::Int Index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(TWT::Int Index);

		static TW3DDescriptorHeap* CreateForRTV(TW3DDevice* Device, TWT::UInt Count);
		static TW3DDescriptorHeap* CreateForDSV(TW3DDevice* Device, TWT::UInt Count);
		static TW3DDescriptorHeap* CreateForSR(TW3DDevice* Device, TWT::UInt Count);

	private:
		ID3D12DescriptorHeap* descriptor_heap;
		TWT::UInt descriptor_count = 0;
		const TWT::UInt IncrementSize;
	};
}