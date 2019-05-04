#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

namespace TW3D {
	class TW3DResourceVB : public TW3DResource {
	public:
		TW3DResourceVB(TW3DDevice* Device, TWT::UInt VertexCount, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL);
		~TW3DResourceVB();

		D3D12_VERTEX_BUFFER_VIEW GetView();
		void UpdateData(const void* Data, TWT::UInt Size);
		TWT::UInt GetVertexCount();
		TWT::UInt GetVertexByteSize();
		TWT::UInt GetSizeInBytes();

	private:
		TW3DTempGCL* temp_gcl;
		TW3DResource* upload_heap;
		D3D12_VERTEX_BUFFER_VIEW view;
		TWT::UInt8* gpu_address = nullptr;
		TWT::UInt single_vertex_size, vertex_count;
	};
}