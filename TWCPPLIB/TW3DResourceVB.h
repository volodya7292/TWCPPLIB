#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceVB : public TW3DResource {
	public:
		TW3DResourceVB(TW3DDevice* Device, TWT::UInt Size, TWT::UInt SingleVertexSize);
		~TW3DResourceVB();

		D3D12_VERTEX_BUFFER_VIEW GetView();
		void UpdateData(const void* Data, TWT::UInt Size);
		TWT::UInt GetVertexCount();
		TWT::UInt GetSizeInBytes();

	private:
		D3D12_VERTEX_BUFFER_VIEW View;
		TWT::UInt8* gpu_address = nullptr;
		TWT::UInt Size, SingleVertexSize, VertexCount = 0;
	};
}