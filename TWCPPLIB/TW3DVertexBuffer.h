#pragma once
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"

namespace TW3D {
	class TW3DVertexBuffer {
	public:
		TW3DVertexBuffer(TW3DDevice* Device, TWT::UInt VertexCount, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL);
		~TW3DVertexBuffer();

		TW3DResourceVB* GetResource();

		void Update(const void* Data, TWT::UInt VertexCount);
		TWT::UInt GetVertexCount();
		TWT::UInt GetSizeInBytes();

	private:
		TW3DResourceVB* vertex_buffer;
	};
}