#pragma once
#include "TW3DDevice.h"
#include "TW3DDescriptorHeap.h"

namespace TW3D {
	class TW3DResource {
	public:
		TW3DResource(TW3DDevice* Device, const CD3DX12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC* ResourceDescription, D3D12_RESOURCE_STATES ResourceStates, const D3D12_CLEAR_VALUE* OptimizedClearValue = nullptr);
		TW3DResource(ID3D12Resource* Resource);
		TW3DResource(TW3DDevice* Device);
		virtual ~TW3DResource();

		ID3D12Resource* Get();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		virtual void Release();

		void Map(TWT::UInt SubResourceIndex, D3D12_RANGE* ReadRange, void** Data);

		static TW3DResource* Create(TW3DDevice* Device, TWT::UInt64 Size, TWT::Bool Staging);
		static TW3DResource* CreateCBStaging(TW3DDevice* Device);

		D3D12_RESOURCE_STATES CurrentState;

	protected:
		TW3DDevice*     Device = nullptr;
		ID3D12Resource* Resource = nullptr;
	};

	D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
}