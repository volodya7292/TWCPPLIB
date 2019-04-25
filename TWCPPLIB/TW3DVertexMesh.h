#pragma once
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"

namespace TW3D {
	class TW3DVertexMesh {
	public:
		TW3DVertexMesh(TW3DResourceManager* ResourceManager, const TWT::Vector<TW3DVertexBuffer*>& VertexBuffers);
		~TW3DVertexMesh();

		TW3DResourceUAV* GetBBBufferResource();
		TW3DResourceUAV* GetMCBufferResource();
		TW3DResourceUAV* GetMCIBufferResource();
		TW3DResourceUAV* GetLBVHNodeBufferResource();
		TW3DResourceUAV* GetLBVHNodeLockBufferResource();

		TWT::UInt GetGVBVertexOffset();
		TWT::UInt GetVertexCount();
		TWT::UInt GetTriangleCount();
		
		TWT::Vector<TW3DVertexBuffer*> VertexBuffers;

	private:
		TW3DResourceUAV* bounding_box_buffer;
		TW3DResourceUAV* morton_codes_buffer;
		TW3DResourceUAV* morton_indices_buffer;
		TW3DResourceUAV* lbvh_node_buffer;
		TW3DResourceUAV* lbvh_node_lock_buffer;
	};

	struct TW3DVertexMeshInstance {
		TW3DVertexMesh* VertexMesh = nullptr;
		TW3DTransform Transform;
	};
}