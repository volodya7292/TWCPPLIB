#pragma once
#include "TW3DDevice.h"

class TW3DDescriptorHeap {
public:
	TW3DDescriptorHeap(TW3DDevice* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, TWT::uint DescriptorCount);
	~TW3DDescriptorHeap();

	ID3D12DescriptorHeap* Get();
	int Allocate();
	void Free(int DescriptorIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int Index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int Index);

	static TW3DDescriptorHeap* CreateForRTV(TW3DDevice* Device, TWT::uint Count);
	static TW3DDescriptorHeap* CreateForDSV(TW3DDevice* Device, TWT::uint Count);
	static TW3DDescriptorHeap* CreateForSR(TW3DDevice* Device, TWT::uint Count);

private:
	static const TWT::uint MAX_DESCRIPTOR_COUNT = 1024;

	ID3D12DescriptorHeap* descriptor_heap;
	bool free_descriptors[MAX_DESCRIPTOR_COUNT];
	const TWT::uint IncrementSize;
};