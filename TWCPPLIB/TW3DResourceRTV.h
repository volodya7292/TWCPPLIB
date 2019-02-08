#pragma once
#include "TW3DResource.h"

namespace TW3D {
	class TW3DResourceRTV : public TW3DResource {
	public:
		TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap);
		~TW3DResourceRTV();

		D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

		void Create(ID3D12Resource* Buffer, TWT::Int Offset);
		void Release();

	private:
		TW3DDescriptorHeap*	DescriptorHeap;
		TWT::Int Offset;
	};
}