#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceVB : public TW3DResource {
	public:
		TW3DResourceVB(TW3DDevice* Device, TWT::UInt64 Size, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL);
		~TW3DResourceVB();

		D3D12_VERTEX_BUFFER_VIEW GetView();

		void UpdateData(BYTE* Data, TWT::UInt64 Size);

	private:
		TW3DTempGCL* TempGCL;
		ID3D12Resource* UploadHeap;
		D3D12_VERTEX_BUFFER_VIEW View;
		TWT::UInt64 Size;
	};
}