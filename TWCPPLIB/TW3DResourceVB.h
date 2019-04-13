#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceVB : public TW3DResource {
	public:
		TW3DResourceVB(TW3DDevice* Device, TWT::UInt Size, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL);
		~TW3DResourceVB();

		D3D12_VERTEX_BUFFER_VIEW GetView();
		void UpdateData(const void* Data, TWT::UInt Size);
		TWT::UInt GetVertexCount();
		TWT::UInt GetSizeInBytes();

	private:
		TW3DTempGCL* TempGCL;
		ID3D12Resource* UploadHeap;
		D3D12_VERTEX_BUFFER_VIEW View;
		TWT::UInt Size, SingleVertexSize, VertexCount;
	};
}