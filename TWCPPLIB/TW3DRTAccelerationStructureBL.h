#pragma once
#include "TW3DResourceVB.h"

namespace TW3D {
	class TW3DRTAccelerationStructureBL
	{
	public:
		TW3DRTAccelerationStructureBL(const TWT::Vector<TW3DResourceVB*>& VertexBuffers);
		~TW3DRTAccelerationStructureBL();

		void Build();
		TWT::UInt64 GetVertexBuffersSize();

	private:
		TWT::Vector<TW3DResourceVB*> vertex_buffers;
	};
}