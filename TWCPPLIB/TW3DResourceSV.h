#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceSV : public TW3DResource {
	public:
		TW3DResourceSV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TW3DTempGCL* TempGCL);
		~TW3DResourceSV();

		void Create2D(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Int Offset);
		void Upload2D(TWT::Byte* Data, TWT::Int64 BytesPerRow);

		static TW3DResourceSV* Create2D(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap, TWT::WString filename, TW3DTempGCL* TempGCL, TWT::Int HeapIndex);

	private:
		TW3DTempGCL* TempGCL;
		TW3DDescriptorHeap* DescriptorHeap;
		D3D12_RESOURCE_DESC ImageDesc = {};
	};
}