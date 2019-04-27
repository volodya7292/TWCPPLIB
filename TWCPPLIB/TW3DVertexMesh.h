#pragma once
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"
#include "TW3DLBVH.h"

namespace TW3D {
	class TW3DVertexMesh {
	public:
		TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers);
		~TW3DVertexMesh();

		TWT::UInt GetVertexCount();
		TWT::UInt GetTriangleCount();
		
		TWT::Vector<TW3DVertexBuffer*> VertexBuffers;
		TW3DLBVH* LBVH;
	};

	struct TW3DVertexMeshInstance {
		TW3DVertexMesh* VertexMesh = nullptr;
		TW3DTransform Transform;
	};
}