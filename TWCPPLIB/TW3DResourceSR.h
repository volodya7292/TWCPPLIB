#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceSR : public TW3DResource {
	public:
		TW3DResourceSR(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TW3DTempGCL* TempGCL);
		~TW3DResourceSR();

		void Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format);
		void CreateArray2D(TWT::UInt Width, TWT::UInt Height, TWT::UInt Depth, DXGI_FORMAT Format);
		void Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

		static TW3DResourceSR* Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TWT::WString filename, TW3DTempGCL* TempGCL);

	private:
		TW3DTempGCL* TempGCL;
		TW3DDescriptorHeap* SRVDescriptorHeap;
		D3D12_RESOURCE_DESC ImageDesc = {};
		TWT::Int SRVIndex;
	};
}