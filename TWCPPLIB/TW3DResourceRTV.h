#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceRTV : public TW3DResource {
	public:
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset);
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TWT::Int RTVOffset, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::Int SRVOffset);
		~TW3DResourceRTV();

		D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

		void Create(ID3D12Resource* Buffer);
		void Create(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format);
		void Release();

	private:
		TW3DDescriptorHeap*	RTVDescriptorHeap;
		TW3DDescriptorHeap* SRVDescriptorHeap;
		TWT::Int RTVOffset, SRVOffset;
	};
}