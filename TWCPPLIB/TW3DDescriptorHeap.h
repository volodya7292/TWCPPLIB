#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DDescriptorHeap {
	public:
		TW3DDescriptorHeap(TW3DDevice* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, TWT::UInt DescriptorCount);
		~TW3DDescriptorHeap();

		ID3D12DescriptorHeap* Get();
		TWT::Int Allocate();
		void Free(TWT::Int DescriptorIndex);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(TWT::Int Index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(TWT::Int Index);

		static TW3DDescriptorHeap* CreateForRTV(TW3DDevice* Device, TWT::UInt Count);
		static TW3DDescriptorHeap* CreateForDSV(TW3DDevice* Device, TWT::UInt Count);
		static TW3DDescriptorHeap* CreateForSR(TW3DDevice* Device, TWT::UInt Count);

	private:
		static const TWT::UInt MAX_DESCRIPTOR_COUNT = 1024;

		ID3D12DescriptorHeap* descriptor_heap;
		TWT::Bool free_descriptors[MAX_DESCRIPTOR_COUNT];
		const TWT::UInt IncrementSize;
	};
}