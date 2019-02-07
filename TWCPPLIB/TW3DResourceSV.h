#pragma once
#include "TW3DResource.h"
#include "TW3DCommandQueue.h"
#include "TW3DFence.h"

namespace TW3D {
	class TW3DGraphicsCommandList;

	class TW3DResourceSV {
	public:
		TW3DResourceSV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap);
		~TW3DResourceSV();

		void Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format);
		void Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow);

		static TW3DResourceSV* Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TWT::WString filename);

	private:
		TW3DDevice* Device;
		TW3DDescriptorHeap* DescriptorHeap;
		TW3DResource* Buffer;
		D3D12_RESOURCE_DESC ImageDesc = {};
	};
}