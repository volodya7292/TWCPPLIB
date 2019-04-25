#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceUAV : public TW3DResource {
	public:
		TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::UInt ElementSizeInBytes);
		TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format);
		~TW3DResourceUAV();

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUUAVHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAVHandle();

		void CreateBuffer(TWT::UInt ElementCount);
		void CreateTexture2D(TWT::UInt Width, TWT::UInt Height);

	private:
		TW3DDescriptorHeap* SRVDescriptorHeap;
		TWT::Int SRVIndex = 0;
		D3D12_RESOURCE_DESC desc;
		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		TWT::UInt element_size;
	};

	D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource = nullptr);
}