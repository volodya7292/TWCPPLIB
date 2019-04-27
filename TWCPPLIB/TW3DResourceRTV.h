#pragma once
#include "TW3DResource.h"
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DResourceRTV : public TW3DResource {
	public:
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap);
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, TWT::Vector4f ClearValue);
		~TW3DResourceRTV();

		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();
		TWT::Vector4f GetClearColor();

		void Create(ID3D12Resource* Buffer);
		void Create(TWT::UInt Width, TWT::UInt Height);
		void Release();

	private:
		TW3DDescriptorHeap*	RTVDescriptorHeap;
		TW3DDescriptorHeap* SRVDescriptorHeap;
		D3D12_RESOURCE_DESC ImageDesc = {};
		TWT::Vector4f       ClearValue;
		TWT::Int RTVIndex = -1, SRVIndex = -1;
	};
}