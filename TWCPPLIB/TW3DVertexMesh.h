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
		TW3DLBVH* GetLBVH();

		void UpdateLBVH(TW3DResourceUAV* GVB, TWT::UInt GVBOffset, TW3DGraphicsCommandList* CommandList = nullptr);

		TWT::Vector<TW3DVertexBuffer*> VertexBuffers;

	private:
		TWT::Bool changed = false;
		TW3DLBVH* lbvh;
	};

	struct TW3DVertexMeshInstance {
		TW3DTransform Transform;
		TW3DVertexMesh* VertexMesh = nullptr;
		SceneLBVHInstance LBVHInstance = {};
		TWT::Bool Changed = false;
		rp3d::RigidBody* RigidBody = nullptr;
	};
}