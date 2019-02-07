#pragma once
#include "TW3DDevice.h"
#include "TW3DDescriptorHeap.h"

namespace TW3D {
	class TW3DResource {
	public:
		TW3DResource(TW3DDevice* Device, const CD3DX12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC* ResourceDescription, D3D12_RESOURCE_STATES ResourceStates, const D3D12_CLEAR_VALUE* OptimizedClearValue = nullptr);
		TW3DResource(ID3D12Resource* resource);
		~TW3DResource();

		ID3D12Resource* Get();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		void Map(TWT::UInt SubResourceIndex, D3D12_RANGE* ReadRange, void** Data);

		static TW3DResource* Create(TW3DDevice* Device, TWT::UInt64 Size, TWT::Bool Staging);
		static TW3DResource* CreateCBStaging(TW3DDevice* Device);
		static TW3DResource* CreateDS(TW3DDevice* Device, TWT::UInt Width, TWT::UInt Height);
		static TW3DResource* CreateTexture2D(TW3DDevice* Device, DXGI_FORMAT Format, TWT::UInt Width, TWT::UInt Height);

		static TW3DResource* CreateDSView(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap);
		static TW3DResource* CreateShaderView(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, DXGI_FORMAT Format);

	private:
		ID3D12Resource* resource;
	};
}