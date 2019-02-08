#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceVB : public TW3DResource {
	public:
		TW3DResourceVB(TW3DDevice* Device, TWT::UInt64 Size, TW3DTempGCL* TempGCL);
		~TW3DResourceVB();

		void UpdateData(BYTE* Data, TWT::UInt64 Size);

	private:
		TW3DTempGCL* TempGCL;
		D3D12_VERTEX_BUFFER_VIEW VertexView;
		TWT::UInt64 Size;
	};
}