#pragma once
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"

namespace TW3D {
	class TW3DVertexMesh {
	public:
		TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers);
		~TW3DVertexMesh();

		TW3DResourceCB* GetCBResource();
		TW3DResourceUAV* GetBBBufferResource();
		TW3DResourceUAV* GetMCBufferResource();
		TWT::UInt64 GetVertexCount();

		TWT::Vector<TW3DVertexBuffer*> VertexBuffers;

	private:
		TW3DResourceCB* constant_buffer;
		TW3DResourceUAV* bounding_box_buffer;
		TW3DResourceUAV* morton_codes_buffer;
	};

	struct TW3DVertexMeshInstance {
		TW3DVertexMesh* VertexMesh = nullptr;
		TW3DTransform Transform;
	};
}