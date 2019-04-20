#pragma once
#include "TW3DVertexBuffer.h"
#include "TW3DTransform.h"

namespace TW3D {
	class TW3DVertexMesh
	{
	public:
		TW3DVertexMesh(const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers);
		~TW3DVertexMesh();

		TWT::UInt64 GetVertexBuffersSize();

		TWT::Vector<TW3DVertexBuffer*> VertexBuffers;
	};

	struct TW3DVertexMeshInstance {
		TW3DVertexMesh* VertexMesh = nullptr;
		TW3DTransform Transform;
	};
}