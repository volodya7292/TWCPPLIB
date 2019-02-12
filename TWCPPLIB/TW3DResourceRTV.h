#pragma once
#include "TW3DResource.h"
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DResourceRTV : public TW3DResource {
	public:
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset);
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset,
			TW3DDescriptorHeap* SRVDescriptorHeap, TWT::Int SRVOffset, DXGI_FORMAT Format, TWT::Vector4f ClearValue);
		~TW3DResourceRTV();

		D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();
		TWT::Vector4f GetClearColor();

		void Create(ID3D12Resource* Buffer);
		void Create(TWT::UInt Width, TWT::UInt Height);
		void Release();

	private:
		TW3DDescriptorHeap*	RTVDescriptorHeap;
		TW3DDescriptorHeap* SRVDescriptorHeap;
		TWT::Int RTVOffset, SRVOffset;
		D3D12_RESOURCE_DESC ImageDesc = {};
		TWT::Vector4f       ClearValue;
	};
}